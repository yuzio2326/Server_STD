#include "D3D11RHIPrivate.h"
#include "Shader.h"

D3D11RHI_API map<type_index, TRefCountPtr<FRHIShader>> RHIShaders;


bool FD3D11DynamicRHI::RHICompileShader(FShaderType* InShaderType, TObjectPtr<FShader>& OutShader)
{

	string TargetName;
	switch (InShaderType->Frequency)
	{
	case SF_Vertex:
		TargetName = "vs";
		break;
	case SF_Pixel:
		TargetName = "ps";
		break;
	default:
		E_LOG(Error, TEXT("아직 지원하지 않는 Frequency 입니다"));
		break;
	}
	TargetName += "_5_0";

	DWORD ShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

	if (WITH_DEBUG)
	{
		// 디버그 정보를 포함하고 최적화를 건너뛰도록 플래그 설정
		ShaderFlags |= D3DCOMPILE_DEBUG;
		ShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
	}

	TRefCountPtr<ID3DBlob> Blob;
	TRefCountPtr<ID3DBlob> ErrorBlob;
	HRESULT Hr = D3DCompileFromFile(InShaderType->ShaderFilePath.data(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, InShaderType->FunctionName.data()
		, TargetName.data(), ShaderFlags, 0, Blob.GetInitReference(), ErrorBlob.GetInitReference());

	if (FAILED(Hr))
	{
		E_LOG(Error, TEXT("{}"), ANSI_TO_TCHAR((char*)ErrorBlob->GetBufferPointer()));
		return false;
	}

	OutShader = InShaderType->ShaderConstructFunction();
	// Reflection
	{
		TRefCountPtr<ID3D11ShaderReflection> Reflector;

		Hr = D3DReflect(
			Blob->GetBufferPointer(),   // 컴파일된 셰이더 코드 포인터
			Blob->GetBufferSize(),      // 셰이더 코드 크기
			IID_ID3D11ShaderReflection,  // 인터페이스 ID
			(void**)Reflector.GetInitReference()// 리플렉터 인터페이스 포인터
		);

		if (FAILED(Hr))
		{
			// 오류 처리
			_ASSERT(false);
			return false;
		}

		D3D11_SHADER_DESC ShaderDesc;
		Reflector->GetDesc(&ShaderDesc);

		for (UINT i = 0; i < ShaderDesc.InputParameters; ++i)
		{
			D3D11_SIGNATURE_PARAMETER_DESC ParamDesc;
			Reflector->GetInputParameterDesc(i, &ParamDesc);
			// 입력 파라미터 정보 사용
		}

		for (UINT i = 0; i < ShaderDesc.BoundResources; ++i)
		{
			D3D11_SHADER_INPUT_BIND_DESC BindDesc;
			Reflector->GetResourceBindingDesc(i, &BindDesc);

			if (BindDesc.Type == D3D_SIT_CBUFFER)
			{
				const FString Name = ANSI_TO_TCHAR(BindDesc.Name);
				FConstantBufferInfo& ConstantBufferInfo = OutShader->ConstantBufferInfos[Name];
				ConstantBufferInfo.Name = Name;
				ConstantBufferInfo.RegisterIndex = BindDesc.BindPoint;
			}
		}

		// 상수 버퍼(cbuffer)의 정보
		for (UINT i = 0; i < ShaderDesc.ConstantBuffers; ++i)
		{
			ID3D11ShaderReflectionConstantBuffer* ConstantBuffer = Reflector->GetConstantBufferByIndex(i);
			D3D11_SHADER_BUFFER_DESC BufferDesc;
			ConstantBuffer->GetDesc(&BufferDesc);
			const FString Name = ANSI_TO_TCHAR(BufferDesc.Name);
			FConstantBufferInfo& ConstantBufferInfo = OutShader->ConstantBufferInfos[Name];
			ConstantBufferInfo.Size = BufferDesc.Size;
			
			// 변수 하나하나의 정보
			/*for (UINT j = 0; j < BufferDesc.Variables; ++j)
			{
				ID3D11ShaderReflectionVariable* Variable = ConstantBuffer->GetVariableByIndex(j);
				D3D11_SHADER_VARIABLE_DESC VarDesc;
				Variable->GetDesc(&VarDesc);
				VarDesc.Size;
			}*/
		}
	}

	// Set shader code
	{
		const uint64 BufferSize = Blob->GetBufferSize();
		OutShader->Code.resize(BufferSize);
		void* Address = Blob->GetBufferPointer();
		memcpy_s(OutShader->Code.data(), OutShader->Code.size(), Address, BufferSize);
	}
	OutShader->Frequency = InShaderType->Frequency;

	return true;
}

FVertexShaderRHIRef FD3D11DynamicRHI::RHICreateVertexShader(const TArray<uint8> Code, const type_index& Key)
{
	if (RHIShaders.contains(Key))
	{
		return (FVertexShaderRHIRef)RHIShaders[Key];
	}

	FD3D11VertexShader* Shader = new FD3D11VertexShader;
	VERIFYD3D11SHADERRESULT(Direct3DDevice->CreateVertexShader(Code.data(), Code.size(), nullptr, Shader->Resource.GetInitReference())
		, Shader, Direct3DDevice);
	Shader->Code = Code;

	FVertexShaderRHIRef ShaderRef = Shader;
	RHIShaders.emplace(Key, ShaderRef);

	return ShaderRef;
}

FPixelShaderRHIRef FD3D11DynamicRHI::RHICreatePixelShader(const TArray<uint8> Code, const type_index& Key)
{
	if (RHIShaders.contains(Key))
	{
		return (FPixelShaderRHIRef)RHIShaders[Key];
	}

	FD3D11PixelShader* Shader = new FD3D11PixelShader;
	VERIFYD3D11SHADERRESULT(Direct3DDevice->CreatePixelShader(Code.data(), Code.size(), nullptr, Shader->Resource.GetInitReference())
		, Shader, Direct3DDevice);

	FPixelShaderRHIRef ShaderRef = Shader;
	RHIShaders.emplace(Key, ShaderRef);

	return ShaderRef;
}

// UE의 FBoundShaderStateLookupKey 대체할 목적으로 만듬
// 우리는 이부분을 간소화 해서 처리 할 예정
struct FBoundShaderStateKey
{
	uint32 Hash;
	FRHIVertexDeclaration* VertexDeclaration;
	FRHIVertexShader* VertexShader;
	FRHIPixelShader* PixelShader;

	explicit FBoundShaderStateKey(FRHIVertexDeclaration* InVertexDeclaration, FRHIVertexShader* InVertexShader, FRHIPixelShader* InPixelShader)
		: VertexDeclaration(InVertexDeclaration), VertexShader(InVertexShader), PixelShader(InPixelShader)
	{
		FD3D11VertexDeclaration* D3D11VertexDeclaration = FD3D11DynamicRHI::ResourceCast(VertexDeclaration);
		Hash = D3D11VertexDeclaration->Hash;

		FD3D11VertexShader* D3D11VertexShader = FD3D11DynamicRHI::ResourceCast(VertexShader);
		FD3D11PixelShader* D3D11PixelShader = FD3D11DynamicRHI::ResourceCast(PixelShader);
		Hash += (uint32)(D3D11VertexShader->Resource.GetReference());
		Hash += (uint32)(D3D11PixelShader->Resource.GetReference());
	}

	bool operator==(const FBoundShaderStateKey& Other) const
	{
		return Hash == Other.Hash;
	}
};

namespace std {
	template<> struct hash<FBoundShaderStateKey> {
		std::size_t operator()(const FBoundShaderStateKey& s) const {
			return s.Hash;
		}
	};
}

typedef unordered_map<FBoundShaderStateKey, FBoundShaderStateRHIRef> FBoundShaderStateCache;
static FBoundShaderStateCache GBoundShaderStateCache;

FD3D11BoundShaderState::FD3D11BoundShaderState(FRHIVertexDeclaration* InVertexDeclarationRHI, FRHIVertexShader* InVertexShaderRHI, FRHIPixelShader* InPixelShaderRHI, ID3D11Device* Direct3DDevice)
{
	FD3D11VertexDeclaration* InVertexDeclaration = FD3D11DynamicRHI::ResourceCast(InVertexDeclarationRHI);
	FD3D11VertexShader* InVertexShader = FD3D11DynamicRHI::ResourceCast(InVertexShaderRHI);
	FD3D11PixelShader* InPixelShader = FD3D11DynamicRHI::ResourceCast(InPixelShaderRHI);
	//FD3D11GeometryShader* InGeometryShader = FD3D11DynamicRHI::ResourceCast(InGeometryShaderRHI);

	if (!InVertexDeclaration)
	{
		_ASSERT(false);
		return;
	}
	memcpy(StreamStrides, InVertexDeclaration->StreamStrides, sizeof(StreamStrides));
	VERIFYD3D11RESULT_EX(
		Direct3DDevice->CreateInputLayout(
			InVertexDeclaration->VertexElements.data(),
			InVertexDeclaration->VertexElements.size(),
			InVertexShader->Code.data(),
			InVertexShader->Code.size(),
			InputLayout.GetInitReference()
		),
		Direct3DDevice
	);

	VertexShader = InVertexShader->Resource;
	PixelShader = InPixelShader ? InPixelShader->Resource : nullptr;
	//GeometryShader = InGeometryShader ? InGeometryShader->Resource : nullptr;
}

FBoundShaderStateRHIRef FD3D11DynamicRHI::RHICreateBoundShaderState(
	FRHIVertexDeclaration* VertexDeclaration, 
	FRHIVertexShader* VertexShader, FRHIPixelShader* PixelShader)
{
	FBoundShaderStateKey Key(VertexDeclaration, VertexShader, PixelShader);

	FBoundShaderStateRHIRef BoundShaderStateRefPtr = nullptr;
	auto It = GBoundShaderStateCache.find(Key);
	if (It == GBoundShaderStateCache.end())
	{
		auto Result = GBoundShaderStateCache.emplace(Key, new FD3D11BoundShaderState(VertexDeclaration, VertexShader, PixelShader, Direct3DDevice));

		BoundShaderStateRefPtr = Result.first->second;
	}
	else
	{
		BoundShaderStateRefPtr = It->second;
	}

	return BoundShaderStateRefPtr;
}