#pragma once
#include "CoreMinimal.h"
#include "RHI.h"
#include "RHIDefinitions.h"
#include "RenderResource.h"

/**
 * 컴파일된 셰이더 및 그 파라미터 바인딩.
 */
class FShader
{
	friend class FShaderType;
	friend class FD3D11DynamicRHI;
	friend class FShaderCompilingManager;
	template<typename ShaderType>
	friend class TShaderMapRef;

public:
	RENDERCORE_API virtual ~FShader();
	inline EShaderFrequency GetFrequency() const { return Frequency; }

	const FConstantBufferInfo& GetConstantBufferInfo(FString InConstantBufferName)
	{
		if (!ConstantBufferInfos.contains(InConstantBufferName))
		{
			E_LOG(Error, TEXT("Failed to find ConstantBufferInfo : {}"), InConstantBufferName); 
		}

		return ConstantBufferInfos[InConstantBufferName];
	}

private:
	EShaderFrequency Frequency;
	TArray<uint8> Code;
	unordered_map<FString, FConstantBufferInfo> ConstantBufferInfos;
};

/**
 * 특정 셰이더 클래스를 직렬화/역직렬화, 컴파일 및 캐시하는 데 사용되는 객체입니다.
 *
 * 셰이더 타입은 EShaderPlatform이나 퍼뮤테이션 ID와 같은 여러 차원에서 여러 FShader 인스턴스를 관리할 수 있습니다.
 * 셰이더 타입의 퍼뮤테이션 수는 단순히 GetPermutationCount()에 의해 제공됩니다.
 */
class FShaderType
{
	using FShaderConstructFunction = function<TObjectPtr<FShader>()>;
	friend class FShaderCompilingManager;
	friend class FD3D11DynamicRHI;

public:
	RENDERCORE_API FShaderType(const type_index ClassType, const TCHAR* InClassName, const FString InShaderFilePath, 
		const char* InFunctionName, EShaderFrequency InShaderFrequency, FShaderConstructFunction InShaderConstructFunction);
	RENDERCORE_API virtual ~FShaderType();

private:
	const type_index TypeIndex;
	FString ClassName;
	FString ShaderFilePath;
	string FunctionName;
	EShaderFrequency Frequency;
	FShaderConstructFunction ShaderConstructFunction;
};

#define DECLARE_SHADER_TYPE(ShaderClass) \
public:\
	static shared_ptr<FShader> ConstructCompiledInstance() { return make_shared<ShaderClass>(); }

/** A macro to implement a shader type. */
#define IMPLEMENT_SHADER_TYPE(ShaderClass,ShaderFilePath,FunctionName,Frequency) \
	static FShaderType StaticType_##ShaderClass(typeid(ShaderClass), TEXT(#ShaderClass), ShaderFilePath, FunctionName, Frequency, ShaderClass::ConstructCompiledInstance);


class RENDERCORE_API FGlobalShaderMap
{
	friend class FShaderType;
	friend class FShaderCompilingManager;
	template<typename ShaderType>
	friend class TShaderMapRef;

private:
	static inline map<type_index, FShaderType*> ShaderTypes; // 컴파일 하기 위한 Shader 정보 등
	static inline map<type_index, TObjectPtr<FShader>> Shaders; // 컴파일된 Shader
};

/**
 * 셰이더 맵에서 요청된 셰이더 타입으로 초기화된 참조.
 */
template<typename ShaderType>
class TShaderMapRef
{
public:
	TShaderMapRef()
	{
		type_index Type = typeid(ShaderType);
		if (FGlobalShaderMap::Shaders.contains(Type))
		{
			auto It = FGlobalShaderMap::Shaders.find(Type);
			ShaderContent = dynamic_cast<ShaderType*>(It->second.get());
		}

		if (!ShaderContent)
		{
			E_LOG(Error, TEXT("해당 Shader를 찾을 수 없습니다 : {}"), ANSI_TO_TCHAR(Type.name()));
		}
	}

	inline FRHIShader* GetRHIShaderBase(EShaderFrequency Frequency) const
	{
		FRHIShader* RHIShader = nullptr;
		if (ShaderContent)
		{
			_ASSERT(ShaderContent->GetFrequency() == Frequency);
			switch (Frequency)
			{
			case SF_Vertex:
				RHIShader = GDynamicRHI->RHICreateVertexShader(ShaderContent->Code, typeid(ShaderType));
				break;
			case SF_Pixel:
				RHIShader = GDynamicRHI->RHICreatePixelShader(ShaderContent->Code, typeid(ShaderType));
				break;
			default:
				_ASSERT(false);
				break;
			}
			_ASSERT(RHIShader->GetFrequency() == Frequency);
		}
		return RHIShader;
	}

	/** @return the shader's vertex shader */
	inline FRHIVertexShader* GetVertexShader() const
	{
		return static_cast<FRHIVertexShader*>(GetRHIShaderBase(SF_Vertex));
	}

	/** @return the shader's pixel shader */
	inline FRHIPixelShader* GetPixelShader() const
	{
		return static_cast<FRHIPixelShader*>(GetRHIShaderBase(SF_Pixel));
	}

	ShaderType* operator->()
	{
		return ShaderContent;
	}

	ShaderType* GetShader() const { return ShaderContent; }

private:
	ShaderType* ShaderContent = nullptr;
};