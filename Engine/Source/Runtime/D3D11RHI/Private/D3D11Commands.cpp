#include "D3D11RHIPrivate.h"
#include "D3D11Viewport.h"
#include <DirectXTex.h>

IRHICommandContext* FD3D11DynamicRHI::RHIGetDefaultContext()
{
	return this;
}

/**
 * 바운드 셰이더 상태를 설정합니다. 이 작업은 버텍스 선언/셰이더 및 픽셀 셰이더를 설정합니다.
 * @param BoundShaderState - 상태 리소스
 */
void FD3D11DynamicRHI::RHISetBoundShaderState(FRHIBoundShaderState* BoundShaderStateRHI)
{
	FD3D11BoundShaderState* BoundShaderState = ResourceCast(BoundShaderStateRHI);

	StateCache.SetStreamStrides(BoundShaderState->StreamStrides);
	StateCache.SetInputLayout(BoundShaderState->InputLayout);
	StateCache.SetVertexShader(BoundShaderState->VertexShader);
	StateCache.SetPixelShader(BoundShaderState->PixelShader);

	//StateCache.SetGeometryShader(BoundShaderState->GeometryShader);

	//// @TODO : really should only discard the constants if the shader state has actually changed.
	//bDiscardSharedConstants = true;

	//// Prevent transient bound shader states from being recreated for each use by keeping a history of the most recently used bound shader states.
	//// The history keeps them alive, and the bound shader state cache allows them to am be reused if needed.
	//BoundShaderStateHistory.Add(BoundShaderState);

	//// Shader changed so all resource tables are dirty
	//DirtyUniformBuffers[SF_Vertex] = 0xffff;
	//DirtyUniformBuffers[SF_Pixel] = 0xffff;
	//DirtyUniformBuffers[SF_Geometry] = 0xffff;

	//// Shader changed.  All UB's must be reset by high level code to match other platforms anway.
	//// Clear to catch those bugs, and bugs with stale UB's causing layout mismatches.
	//// Release references to bound uniform buffers.
	//for (int32 Frequency = 0; Frequency < SF_NumStandardFrequencies; ++Frequency)
	//{
	//	for (int32 BindIndex = 0; BindIndex < MAX_UNIFORM_BUFFERS_PER_SHADER_STAGE; ++BindIndex)
	//	{
	//		BoundUniformBuffers[Frequency][BindIndex] = nullptr;
	//	}
	//}

	//if (GUnbindResourcesBetweenDrawsInDX11 || GRHIGlobals.IsDebugLayerEnabled)
	//{
	//	ClearAllShaderResources();
	//}
}

FBufferRHIRef FD3D11DynamicRHI::RHICreateBuffer(FRHICommandList& RHICmdList, FRHIBufferDesc const& BufferDesc, ERHIAccess ResourceState, FRHIResourceCreateInfo& CreateInfo)
{
	if (BufferDesc.IsNull())
	{
		return new FD3D11Buffer(nullptr, BufferDesc);
	}

	// CreateBuffer가 명확하지 않게 실패하기 전에 크기가 0이 아닌지 명시적으로 확인합니다.
	_ASSERT(BufferDesc.Size > 0, TEXT("크기 0으로 버퍼 '%s'를 생성하려고 시도했습니다."), CreateInfo.DebugName ? CreateInfo.DebugName : TEXT("(null)"));
	
	// 버퍼를 설명합니다.
	D3D11_BUFFER_DESC Desc{};
	Desc.ByteWidth = BufferDesc.Size;

	if (EnumHasAnyFlags(BufferDesc.Usage, BUF_AnyDynamic))
	{
		Desc.Usage = D3D11_USAGE_DYNAMIC;
		Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}

	if (EnumHasAnyFlags(BufferDesc.Usage, BUF_VertexBuffer))
	{
		Desc.BindFlags |= D3D11_BIND_VERTEX_BUFFER;
	}

	if (EnumHasAnyFlags(BufferDesc.Usage, BUF_IndexBuffer))
	{
		Desc.BindFlags |= D3D11_BIND_INDEX_BUFFER;
	}

	if (EnumHasAnyFlags(BufferDesc.Usage, BUF_ByteAddressBuffer))
	{
		Desc.MiscFlags |= D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
	}
	else if (EnumHasAnyFlags(BufferDesc.Usage, BUF_StructuredBuffer))
	{
		_ASSERT(false);
		Desc.StructureByteStride = BufferDesc.Stride;
		Desc.MiscFlags |= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	}

	if (EnumHasAnyFlags(BufferDesc.Usage, BUF_ShaderResource))
	{
		Desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
	}

	if (EnumHasAnyFlags(BufferDesc.Usage, BUF_UnorderedAccess))
	{
		Desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
	}

	if (EnumHasAnyFlags(BufferDesc.Usage, BUF_DrawIndirect))
	{
		Desc.MiscFlags |= D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
	}

	if (EnumHasAnyFlags(BufferDesc.Usage, BUF_Shared))
	{
		/*if (GCVarUseSharedKeyedMutex->GetInt() != 0)
		{
			Desc.MiscFlags |= D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;
		}
		else*/
		{
			Desc.MiscFlags |= D3D11_RESOURCE_MISC_SHARED;
		}
	}

	// If a resource array was provided for the resource, create the resource pre-populated
	D3D11_SUBRESOURCE_DATA InitData;
	D3D11_SUBRESOURCE_DATA* pInitData = NULL;
	if (CreateInfo.ResourceArray)
	{
		_ASSERT(BufferDesc.Size == CreateInfo.ResourceArray->GetResourceDataSize());
		InitData.pSysMem = CreateInfo.ResourceArray->GetResourceData();
		InitData.SysMemPitch = BufferDesc.Size;
		InitData.SysMemSlicePitch = 0;
		pInitData = &InitData;
	}

	TRefCountPtr<ID3D11Buffer> BufferResource;
	{
		HRESULT hr = Direct3DDevice->CreateBuffer(&Desc, pInitData, BufferResource.GetInitReference());
		if (FAILED(hr))
		{
			/*E_LOG(Error, TEXT("Failed to create buffer '{}' with ByteWidth={}, Usage={}, BindFlags=0x{:x}, CPUAccessFlags=0x{:x}, MiscFlags=0x{:x}, StructureByteStride={}, InitData=0x{:x}"),
				CreateInfo.DebugName ? CreateInfo.DebugName : TEXT(""), Desc.ByteWidth, Desc.Usage, Desc.BindFlags, Desc.CPUAccessFlags, Desc.MiscFlags, Desc.StructureByteStride, pInitData);*/
			VerifyD3D11Result(hr, "CreateBuffer", __FILE__, __LINE__, Direct3DDevice);
			_ASSERT(false);
		}
	}

	FString DebugName = CreateInfo.DebugName;
	if (CreateInfo.DebugName)
	{
		BufferResource->SetPrivateData(WKPDID_D3DDebugObjectName, wcslen(CreateInfo.DebugName) + 1, TCHAR_TO_ANSI(DebugName).data());
	}

	/*if (CreateInfo.DebugName)
	{
		BufferResource->SetPrivateData(WKPDID_D3DDebugObjectName, wcslen(CreateInfo.DebugName) + 1, TCHAR_TO_ANSI(DebugName).data());
	}*/

	if (CreateInfo.ResourceArray)
	{
		// 리소스 배열의 내용을 폐기합니다.
		CreateInfo.ResourceArray->Discard();
	}

	FD3D11Buffer* NewBuffer = new FD3D11Buffer(BufferResource, BufferDesc);
	if (CreateInfo.DebugName)
	{
		NewBuffer->SetName(CreateInfo.DebugName);
	}

	//D3D11BufferStats::UpdateBufferStats(*NewBuffer, true);

	return NewBuffer;
}

void FD3D11DynamicRHI::RHISetStreamSource(uint32 StreamIndex, FRHIBuffer* VertexBufferRHI, uint32 Offset)
{
	FD3D11Buffer* VertexBuffer = ResourceCast(VertexBufferRHI);

	ID3D11Buffer* D3DBuffer = VertexBuffer ? VertexBuffer->Resource.GetReference() : nullptr;
	TrackResourceBoundAsVB(VertexBuffer, StreamIndex);
	StateCache.SetStreamSource(D3DBuffer, StreamIndex, Offset);
}

static D3D11_PRIMITIVE_TOPOLOGY GetD3D11PrimitiveType(EPrimitiveType PrimitiveType)
{
	switch (PrimitiveType)
	{
	case PT_TriangleList: return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	case PT_TriangleStrip: return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	case PT_LineList: return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	case PT_PointList: return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;

	default: E_LOG(Fatal, TEXT("Unknown primitive type: {}"), (int32)PrimitiveType);
	};

	return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}

void FD3D11DynamicRHI::RHISetPrimitiveTopology(EPrimitiveType InPrimitiveType)
{
	PrimitiveType = InPrimitiveType;
	//StateCache.SetPrimitiveTopology(GetD3D11PrimitiveType(InPrimitiveType));
}


void FD3D11DynamicRHI::RHIDrawPrimitive(uint32 BaseVertexIndex, uint32 NumPrimitives, uint32 NumInstances)
{
	// TODO
	//CommitGraphicsResourceTables();
	//CommitNonComputeShaderConstants();

	uint32 VertexCount = GetVertexCountForPrimitiveCount(NumPrimitives, PrimitiveType);

	//GPUProfilingData.RegisterGPUWork(NumPrimitives * NumInstances, VertexCount * NumInstances);
	StateCache.SetPrimitiveTopology(GetD3D11PrimitiveType(PrimitiveType));
	if (NumInstances > 1)
	{
		Direct3DDeviceIMContext->DrawInstanced(VertexCount, NumInstances, BaseVertexIndex, 0);
	}
	else
	{
		Direct3DDeviceIMContext->Draw(VertexCount, BaseVertexIndex);
	}

	//EnableUAVOverlap();
}


// Rasterizer state.
void FD3D11DynamicRHI::RHISetRasterizerState(FRHIRasterizerState* NewStateRHI)
{
	FD3D11RasterizerState* NewState = ResourceCast(NewStateRHI);
	StateCache.SetRasterizerState(NewState->Resource);
}

void FD3D11DynamicRHI::ValidateExclusiveDepthStencilAccess(FExclusiveDepthStencil RequestedAccess) const
{
	const bool bSrcDepthWrite = RequestedAccess.IsDepthWrite();
	const bool bSrcStencilWrite = RequestedAccess.IsStencilWrite();

	if (bSrcDepthWrite || bSrcStencilWrite)
	{
		// 새로운 규칙: SetRenderTarget[s]()를 먼저 호출해야 합니다
		_ASSERT(CurrentDepthTexture);

		const bool bDstDepthWrite = CurrentDSVAccessType.IsDepthWrite();
		const bool bDstStencilWrite = CurrentDSVAccessType.IsStencilWrite();

		// 요청된 접근이 불가능합니다. SetRenderTarget의 EExclusiveDepthStencil을 수정하거나 다른 것을 요청하십시오.
		_ASSERT(
			!bSrcDepthWrite || bDstDepthWrite,
			TEXT("기대값: SrcDepthWrite := false 또는 DstDepthWrite := true. 실제값: SrcDepthWrite := %s 또는 DstDepthWrite := %s"),
			(bSrcDepthWrite) ? TEXT("true") : TEXT("false"),
			(bDstDepthWrite) ? TEXT("true") : TEXT("false")
		);

		_ASSERT(
			!bSrcStencilWrite || bDstStencilWrite,
			TEXT("기대값: SrcStencilWrite := false 또는 DstStencilWrite := true. 실제값: SrcStencilWrite := %s 또는 DstStencilWrite := %s"),
			(bSrcStencilWrite) ? TEXT("true") : TEXT("false"),
			(bDstStencilWrite) ? TEXT("true") : TEXT("false")
		);

	}
}

void FD3D11DynamicRHI::RHISetDepthStencilState(FRHIDepthStencilState* NewStateRHI, uint32 StencilRef)
{
	FD3D11DepthStencilState* NewState = ResourceCast(NewStateRHI);

	ValidateExclusiveDepthStencilAccess(NewState->AccessType);

	StateCache.SetDepthStencilState(NewState->Resource, StencilRef);
}

void FD3D11DynamicRHI::RHIDrawIndexedPrimitive(FRHIBuffer* IndexBufferRHI, int32 BaseVertexIndex, uint32 FirstInstance, uint32 NumVertices, uint32 StartIndex, uint32 NumPrimitives, uint32 NumInstances)
{
	// 임시로 컬링하지 않도록 설정
	//{
	//	D3D11_RASTERIZER_DESC RasterDesc;
	//	ZeroMemory(&RasterDesc, sizeof(RasterDesc));
	//	RasterDesc.FillMode = D3D11_FILL_SOLID;
	//	RasterDesc.CullMode = D3D11_CULL_BACK;
	//	RasterDesc.FrontCounterClockwise = true; // true: CCW를 앞면으로 가정(UE에서 이걸 기본값으로 사용 중: FD3D11DynamicRHI::RHICreateRasterizerState)
	//	RasterDesc.DepthClipEnable = false;
	//	RasterDesc.MultisampleEnable = true;

	//	TRefCountPtr<ID3D11RasterizerState> RasterState;
	//	Direct3DDevice->CreateRasterizerState(&RasterDesc, RasterState.GetInitReference());
	//	Direct3DDeviceIMContext->RSSetState(RasterState);
	//}
	// [깊이 반전(근평면 1.f)] 임시로 여기서 일괄 처리
	//{
	//	// 반전된 깊이 스텐실 상태 설정
	//	D3D11_DEPTH_STENCIL_DESC invertedDepthStencilDesc;
	//	ZeroMemory(&invertedDepthStencilDesc, sizeof(invertedDepthStencilDesc));
	//	invertedDepthStencilDesc.DepthEnable = TRUE;
	//	invertedDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	//	invertedDepthStencilDesc.DepthFunc = D3D11_COMPARISON_GREATER; // 반전된 깊이 비교 함수

	//	TRefCountPtr<ID3D11DepthStencilState> invertedDepthStencilState;
	//	Direct3DDevice->CreateDepthStencilState(&invertedDepthStencilDesc, invertedDepthStencilState.GetInitReference());
	//	Direct3DDeviceIMContext->OMSetDepthStencilState(invertedDepthStencilState, 1);
	//}
	FD3D11Buffer* IndexBuffer = ResourceCast(IndexBufferRHI);
	
	// 호출된 함수는 입력이 유효한지 확인해야 합니다. 이는 숨겨진 버그를 방지합니다.
	_ASSERT(NumPrimitives > 0);

	//CommitGraphicsResourceTables();
	//CommitNonComputeShaderConstants();

	// determine 16bit vs 32bit indices
	const DXGI_FORMAT Format = (IndexBuffer->GetStride() == sizeof(uint16) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT);

	uint32 IndexCount = GetVertexCountForPrimitiveCount(NumPrimitives, PrimitiveType);

	// 인덱스 버퍼 범위 밖을 읽으려고 하지 않는지 확인합니다.
	// 테스트는 다음의 최적화된 버전입니다: StartIndex + IndexCount <= IndexBuffer->GetSize() / IndexBuffer->GetStride()
	_ASSERT((StartIndex + IndexCount) * IndexBuffer->GetStride() <= IndexBuffer->GetSize(),
		TEXT("Start %u, Count %u, Type %u, Buffer Size %u, Buffer stride %u"), StartIndex, IndexCount, PrimitiveType, IndexBuffer->GetSize(), IndexBuffer->GetStride());

	TrackResourceBoundAsIB(IndexBuffer);
	StateCache.SetIndexBuffer(IndexBuffer->Resource, Format, 0);
	StateCache.SetPrimitiveTopology(GetD3D11PrimitiveType(PrimitiveType));

	if (NumInstances > 1 || FirstInstance != 0)
	{
		const uint64 TotalIndexCount = (uint64)NumInstances * (uint64)IndexCount + (uint64)StartIndex;
		_ASSERT(TotalIndexCount <= (uint64)0xFFFFFFFF, TEXT("Instanced Index Draw exceeds maximum d3d11 limit: Total: %llu, NumInstances: %llu, IndexCount: %llu, StartIndex: %llu, FirstInstance: %llu"), TotalIndexCount, NumInstances, IndexCount, StartIndex, FirstInstance);
		Direct3DDeviceIMContext->DrawIndexedInstanced(IndexCount, NumInstances, StartIndex, BaseVertexIndex, FirstInstance);
	}
	else
	{
		Direct3DDeviceIMContext->DrawIndexed(IndexCount, StartIndex, BaseVertexIndex);
	}

	//EnableUAVOverlap();
}

static void ValidateScissorRect(const D3D11_VIEWPORT& Viewport, const D3D11_RECT& ScissorRect)
{
	_ASSERT(ScissorRect.left >= (LONG)Viewport.TopLeftX);
	_ASSERT(ScissorRect.top >= (LONG)Viewport.TopLeftY);
	_ASSERT(ScissorRect.right <= (LONG)Viewport.TopLeftX + (LONG)Viewport.Width);
	_ASSERT(ScissorRect.bottom <= (LONG)Viewport.TopLeftY + (LONG)Viewport.Height);
	_ASSERT(ScissorRect.left <= ScissorRect.right && ScissorRect.top <= ScissorRect.bottom);
}

void FD3D11DynamicRHI::RHISetViewport(float MinX, float MinY, float MinZ, float MaxX, float MaxY, float MaxZ)
{
	// These are the maximum viewport extents for D3D11. Exceeding them leads to badness.
	_ASSERT(MinX <= (float)D3D11_VIEWPORT_BOUNDS_MAX);
	_ASSERT(MinY <= (float)D3D11_VIEWPORT_BOUNDS_MAX);
	_ASSERT(MaxX <= (float)D3D11_VIEWPORT_BOUNDS_MAX);
	_ASSERT(MaxY <= (float)D3D11_VIEWPORT_BOUNDS_MAX);

	D3D11_VIEWPORT Viewport = { MinX, MinY, MaxX - MinX, MaxY - MinY, MinZ, MaxZ };
	//avoid setting a 0 extent viewport, which the debug runtime doesn't like
	if (Viewport.Width > 0 && Viewport.Height > 0)
	{
		StateCache.SetViewport(Viewport);
		RHISetScissorRect(true, MinX, MinY, MaxX, MaxY);
	}
}

void FD3D11DynamicRHI::RHISetScissorRect(bool bEnable, uint32 MinX, uint32 MinY, uint32 MaxX, uint32 MaxY)
{
	D3D11_VIEWPORT Viewport;
	StateCache.GetViewport(&Viewport);

	D3D11_RECT ScissorRect;
	if (bEnable)
	{
		ScissorRect.left = MinX;
		ScissorRect.top = MinY;
		ScissorRect.right = MaxX;
		ScissorRect.bottom = MaxY;
	}
	else
	{
		ScissorRect.left = (LONG)Viewport.TopLeftX;
		ScissorRect.top = (LONG)Viewport.TopLeftY;
		ScissorRect.right = (LONG)Viewport.TopLeftX + (LONG)Viewport.Width;
		ScissorRect.bottom = (LONG)Viewport.TopLeftY + (LONG)Viewport.Height;
	}

	ValidateScissorRect(Viewport, ScissorRect);
	Direct3DDeviceIMContext->RSSetScissorRects(1, &ScissorRect);
}

unordered_map<FString, FTextureRHIRef> Textures;

FTextureRHIRef FD3D11DynamicRHI::RHICreateTexture(const FRHITextureCreateDesc& CreateDesc)
{
	const FString TextureName = CreateDesc.DebugName;
	if (!TextureName.empty())
	{
		if (Textures.contains(TextureName))
		{
			FTextureRHIRef Texture = Textures[TextureName];

			FD3D11Texture* D3D11Texture = ResourceCast(Texture);
			const FVector3D TextureSize = D3D11Texture->GetSizeXYZ();
			const FVector3D NewTextureSize = CreateDesc.GetSize();
			if (TextureSize != CreateDesc.GetSize())
			{
				E_LOG(Warning, TEXT("{} size changed: {} {} {} to {} {} {}"), 
					TextureName, 
					TextureSize.x, TextureSize.y, TextureSize.z,
					NewTextureSize.x, NewTextureSize.y, NewTextureSize.z);
				Textures.erase(TextureName);
			}
			else
			{
				return Texture;
			}
		}
	}

	FTextureRHIRef NewTexture = CreateDesc.IsTexture3D()
		? nullptr // CreateD3D11Texture3D(CreateDesc)
		: CreateD3D11Texture2D(CreateDesc);

	if (!TextureName.empty())
	{
		Textures.emplace(TextureName, NewTexture);
	}

	return NewTexture;
}

class FTextureBulkData : public FResourceBulkDataInterface
{
public:
	FTextureBulkData() = default;
	FTextureBulkData(DirectX::ScratchImage& InImage)
		: ImageBulkData(InImage.GetPixels()), Size(InImage.GetPixelsSize()) 
	{
	}

	/**
	 * @return 미리 할당된 리소스 메모리에 대한 포인터를 반환합니다.
	 */
	virtual const void* GetResourceBulkData() const
	{
		return ImageBulkData;
	}

	/**
	 * @return 리소스 메모리의 크기를 반환합니다.
	 */
	virtual uint32 GetResourceBulkDataSize() const
	{
		return Size;
	}

	/**
	 * RHI 리소스를 초기화한 후 메모리를 해제합니다.
	 */
	virtual void Discard()
	{
		ImageBulkData = nullptr;
		Size = 0;
	}

private:
	uint8* ImageBulkData = nullptr;
	uint32 Size = 0;
};

FString ChangeFileExtension(const FString& filePath, const FString& newExtension)
{
	FString::size_type dotPos = filePath.find_last_of(TEXT("."));
	if (dotPos == std::string::npos) 
	{
		// 확장자가 없을 경우 그냥 새로운 확장자를 덧붙임
		return filePath + newExtension;
	}
	// 확장자를 변경한 새로운 파일 경로 반환
	return filePath.substr(0, dotPos) + newExtension;
}

EPixelFormat ConvertDXGIFormatToUEFormat(DXGI_FORMAT dxgiFormat)
{
	switch (dxgiFormat)
	{
	case DXGI_FORMAT_R8G8B8A8_UNORM:
		return PF_R8G8B8A8;
	case DXGI_FORMAT_B8G8R8A8_UNORM:
		return PF_B8G8R8A8;
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
		return PF_FloatRGBA;
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
		return PF_A32B32G32R32F;
	case DXGI_FORMAT_D24_UNORM_S8_UINT:
		return PF_DepthStencil;
	case DXGI_FORMAT_R32_FLOAT:
		return PF_R32_FLOAT;
	case DXGI_FORMAT_R8_UNORM:
		return PF_G8;
	case DXGI_FORMAT_R16G16_FLOAT:
		return PF_G16R16F;
	case DXGI_FORMAT_R16G16B16A16_UNORM:
		return PF_R16G16B16A16_UNORM;
	case DXGI_FORMAT_BC1_UNORM:
		return PF_DXT1;
	case DXGI_FORMAT_BC2_UNORM:
		return PF_DXT3;
	case DXGI_FORMAT_BC3_UNORM:
		return PF_DXT5;
	case DXGI_FORMAT_R16_UNORM:
		return PF_R16_UINT;
	case DXGI_FORMAT_R11G11B10_FLOAT:
		return PF_FloatR11G11B10;
	case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
		return PF_B8G8R8A8;
	default:
		_ASSERT(false); // 필요하면 추가 할 것
		return PF_Unknown;
	}
}

FTextureRHIRef FD3D11DynamicRHI::RHICreateTexture(const FString& InFilePath, const FString& InExtension)
{
	if (Textures.contains(InFilePath))
	{
		return Textures[InFilePath];
	}

	if (InExtension.empty())
	{
		E_LOG(Error, TEXT("확장자가 없는 파일 경로: {}"), InFilePath);
		return FTextureRHIRef();
	}

	DirectX::ScratchImage* ScratchImage = nullptr;
	DirectX::ScratchImage Image;
	DirectX::ScratchImage GeneratedWIthMipImage;
	FTextureBulkData BulkData;

	if (InExtension == TEXT("png"))
	{
		HRESULT Hr = DirectX::LoadFromWICFile(InFilePath.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, Image);
		if (FAILED(Hr))
		{
			E_LOG(Error, TEXT("Failed to Load From WIC File: {}"), InFilePath);
			return FTextureRHIRef();
		}
	}
	else if (InExtension == TEXT("dds"))
	{
		HRESULT Hr = DirectX::LoadFromDDSFile(InFilePath.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, Image);
		if (FAILED(Hr))
		{
			E_LOG(Error, TEXT("Failed to Load From DDS File: {}"), InFilePath);
			return FTextureRHIRef();
		}
	}
	else
	{
		E_LOG(Error, TEXT("지원하지 않는 Texture Format: {}"), InFilePath);
	}

	{
		const DirectX::TexMetadata& Meta = Image.GetMetadata();
		if (Meta.mipLevels == 1 && Meta.width >= 16 && Meta.height >= 16)
		{
			HRESULT Hr = DirectX::GenerateMipMaps(Image.GetImages(), Image.GetImageCount(), Image.GetMetadata(), DirectX::TEX_FILTER_DEFAULT, 0, GeneratedWIthMipImage);
			if (FAILED(Hr))
			{
				E_LOG(Error, TEXT("Failed to GenerateMipMaps : {}"), InFilePath);
				return FTextureRHIRef();
			}

			FString NewFilePath = ChangeFileExtension(InFilePath, TEXT(".dds"));
			Hr = DirectX::SaveToDDSFile(GeneratedWIthMipImage.GetImages(), GeneratedWIthMipImage.GetMetadata().mipLevels, GeneratedWIthMipImage.GetMetadata(), DirectX::DDS_FLAGS_NONE, NewFilePath.c_str());
			if (FAILED(Hr))
			{
				E_LOG(Error, TEXT("Failed to SaveToDDSFile : {}"), InFilePath);
				return FTextureRHIRef();
			}
			E_LOG(Warning, TEXT("MipMap을 추가한 다음 DDS File을 사용하세요. {}"), NewFilePath);
			ScratchImage = &GeneratedWIthMipImage;
		}
		else
		{
			ScratchImage = &Image;
		}
	}

	BulkData = FTextureBulkData(*ScratchImage);

	const DirectX::TexMetadata& Meta = ScratchImage->GetMetadata();
	const EPixelFormat Format = ConvertDXGIFormatToUEFormat(Meta.format);

	const FRHITextureCreateDesc Desc =
		FRHITextureCreateDesc::Create2D(InFilePath.data())
		.SetExtent(Meta.width, Meta.height)
		.SetFormat(Format)
		.SetNumMips(Meta.mipLevels)
		.SetFlags(TexCreate_ShaderResource)
		.SetBulkData(&BulkData);

	FTextureRHIRef NewTexture = RHICreateTexture(Desc);
	return NewTexture;
}

FTextureRHIRef FD3D11DynamicRHI::RHICreateCubeTexture(const FString& InFilePath, const FString& InExtension)
{
	if (Textures.contains(InFilePath))
	{
		return Textures[InFilePath];
	}

	if (InExtension != TEXT("dds"))
	{
		E_LOG(Error, TEXT("지원하지 않는 CubeTexture Format: {}"), InFilePath);
		return FTextureRHIRef();
	}

	DirectX::ScratchImage Image;

	HRESULT Hr = DirectX::LoadFromDDSFile(InFilePath.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, Image);
	if (FAILED(Hr))
	{
		E_LOG(Error, TEXT("Failed to Load From DDS File: {}"), InFilePath);
		return FTextureRHIRef();
	}
	FTextureBulkData BulkData(Image);

	const DirectX::TexMetadata& Meta = Image.GetMetadata();
	const EPixelFormat Format = ConvertDXGIFormatToUEFormat(Meta.format);
	const FRHITextureCreateDesc Desc =
		FRHITextureCreateDesc::CreateCube(InFilePath.data())
		.SetExtent(Meta.width, Meta.height)
		.SetFormat(Format)
		.SetNumMips(Meta.mipLevels)
		.SetFlags(TexCreate_ShaderResource)
		.SetBulkData(&BulkData);

	FTextureRHIRef NewTexture = RHICreateTexture(Desc);
	return NewTexture;
}

struct FRTVDesc
{
	uint32 Width;
	uint32 Height;
	DXGI_SAMPLE_DESC SampleDesc;
};

// RTV의 mip 레벨에 맞게 너비와 높이 치수가 조정된 FRTVDesc 구조체를 반환합니다.
FRTVDesc GetRenderTargetViewDesc(ID3D11RenderTargetView* RenderTargetView)
{
	D3D11_RENDER_TARGET_VIEW_DESC TargetDesc;
	RenderTargetView->GetDesc(&TargetDesc);

	TRefCountPtr<ID3D11Resource> BaseResource;
	RenderTargetView->GetResource((ID3D11Resource**)BaseResource.GetInitReference());
	uint32 MipIndex = 0;
	FRTVDesc ret;
	memset(&ret, 0, sizeof(ret));

	switch (TargetDesc.ViewDimension)
	{
	case D3D11_RTV_DIMENSION_TEXTURE2D:
	case D3D11_RTV_DIMENSION_TEXTURE2DMS:
	case D3D11_RTV_DIMENSION_TEXTURE2DARRAY:
	case D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY:
	{
		D3D11_TEXTURE2D_DESC Desc;
		((ID3D11Texture2D*)(BaseResource.GetReference()))->GetDesc(&Desc);
		ret.Width = Desc.Width;
		ret.Height = Desc.Height;
		ret.SampleDesc = Desc.SampleDesc;
		if (TargetDesc.ViewDimension == D3D11_RTV_DIMENSION_TEXTURE2D || TargetDesc.ViewDimension == D3D11_RTV_DIMENSION_TEXTURE2DARRAY)
		{
			// 모든 비멀티샘플 텍스처 유형은 동일한 위치에 mip 슬라이스를 가지고 있습니다.
			MipIndex = TargetDesc.Texture2D.MipSlice;
		}
		break;
	}
	case D3D11_RTV_DIMENSION_TEXTURE3D:
	{
		D3D11_TEXTURE3D_DESC Desc;
		((ID3D11Texture3D*)(BaseResource.GetReference()))->GetDesc(&Desc);
		ret.Width = Desc.Width;
		ret.Height = Desc.Height;
		ret.SampleDesc.Count = 1;
		ret.SampleDesc.Quality = 0;
		MipIndex = TargetDesc.Texture3D.MipSlice;
		break;
	}
	default:
	{
		// not expecting 1D targets.
		_ASSERT(false);
		//checkNoEntry();
	}
	}
	ret.Width >>= MipIndex;
	ret.Height >>= MipIndex;
	return ret;
}

void FD3D11DynamicRHI::SetRenderTargets(
	uint32 NewNumSimultaneousRenderTargets,
	const FRHIRenderTargetView* NewRenderTargetsRHI,
	const FRHIDepthRenderTargetView* NewDepthStencilTargetRHI)
{
	FD3D11Texture* NewDepthStencilTarget = ResourceCast(NewDepthStencilTargetRHI ? NewDepthStencilTargetRHI->Texture : nullptr);

	_ASSERT(NewNumSimultaneousRenderTargets <= MaxSimultaneousRenderTargets);

	bool bTargetChanged = false;

	// 깊이 쓰기가 활성화되어 있는지 여부에 따라 적절한 깊이 스텐실 뷰를 설정합니다.
	ID3D11DepthStencilView* DepthStencilView = NULL;
	if (NewDepthStencilTarget)
	{
		_ASSERT(NewDepthStencilTargetRHI);
		CurrentDSVAccessType = NewDepthStencilTargetRHI->GetDepthStencilAccess();
		DepthStencilView = NewDepthStencilTarget->GetDepthStencilView(CurrentDSVAccessType);

		// 바인딩된 깊이 스텐실 타겟의 셰이더 뷰를 언바인딩합니다.
		ConditionalClearShaderResource(NewDepthStencilTarget, false);
	}

	// 깊이 스텐실 타겟이 이전 상태와 다른지 확인합니다.
	if (CurrentDepthStencilTarget != DepthStencilView)
	{
		CurrentDepthTexture = NewDepthStencilTarget;
		CurrentDepthStencilTarget = DepthStencilView;
		bTargetChanged = true;
	}

	// 새로운 렌더 타겟들을 위한 렌더 타겟 뷰를 모읍니다.
	ID3D11RenderTargetView* NewRenderTargetViews[MaxSimultaneousRenderTargets];
	for (uint32 RenderTargetIndex = 0; RenderTargetIndex < MaxSimultaneousRenderTargets; ++RenderTargetIndex)
	{
		ID3D11RenderTargetView* RenderTargetView = NULL;
		if (RenderTargetIndex < NewNumSimultaneousRenderTargets && NewRenderTargetsRHI[RenderTargetIndex].Texture != nullptr)
		{
			int32 RTMipIndex = NewRenderTargetsRHI[RenderTargetIndex].MipIndex;
			int32 RTSliceIndex = NewRenderTargetsRHI[RenderTargetIndex].ArraySliceIndex;

			FD3D11Texture* NewRenderTarget = ResourceCast(NewRenderTargetsRHI[RenderTargetIndex].Texture);
			RenderTargetView = NewRenderTarget ? NewRenderTarget->GetRenderTargetView(RTMipIndex, RTSliceIndex) : nullptr;

			_ASSERT(RenderTargetView, TEXT("Texture being set as render target has no RTV"));

			// 바인딩된 렌더 타겟의 셰이더 뷰를 언바인딩합니다.
			ConditionalClearShaderResource(NewRenderTarget, false);

#if UE_BUILD_DEBUG	
			// A check to allow you to pinpoint what is using mismatching targets
			// We filter our d3ddebug spew that checks for this as the d3d runtime's check is wrong.
			// For filter code, see D3D11Device.cpp look for "OMSETRENDERTARGETS_INVALIDVIEW"
			if (RenderTargetView && DepthStencilView)
			{
				FRTVDesc RTTDesc = GetRenderTargetViewDesc(RenderTargetView);

				TRefCountPtr<ID3D11Texture2D> DepthTargetTexture;
				DepthStencilView->GetResource((ID3D11Resource**)DepthTargetTexture.GetInitReference());

				D3D11_TEXTURE2D_DESC DTTDesc;
				DepthTargetTexture->GetDesc(&DTTDesc);

				// enforce color target is <= depth and MSAA settings match
				if (RTTDesc.Width > DTTDesc.Width || RTTDesc.Height > DTTDesc.Height ||
					RTTDesc.SampleDesc.Count != DTTDesc.SampleDesc.Count ||
					RTTDesc.SampleDesc.Quality != DTTDesc.SampleDesc.Quality)
				{
					UE_LOG(LogD3D11RHI, Fatal, TEXT("RTV(%i,%i c=%i,q=%i) and DSV(%i,%i c=%i,q=%i) have mismatching dimensions and/or MSAA levels!"),
						RTTDesc.Width, RTTDesc.Height, RTTDesc.SampleDesc.Count, RTTDesc.SampleDesc.Quality,
						DTTDesc.Width, DTTDesc.Height, DTTDesc.SampleDesc.Count, DTTDesc.SampleDesc.Quality);
				}
			}
#endif
		}

		NewRenderTargetViews[RenderTargetIndex] = RenderTargetView;

		// Check if the render target is different from the old state.
		if (CurrentRenderTargets[RenderTargetIndex] != RenderTargetView)
		{
			CurrentRenderTargets[RenderTargetIndex] = RenderTargetView;
			bTargetChanged = true;
		}
	}
	if (NumSimultaneousRenderTargets != NewNumSimultaneousRenderTargets)
	{
		NumSimultaneousRenderTargets = NewNumSimultaneousRenderTargets;
		uint32 Bit = 1;
		uint32 Mask = 0;
		for (uint32 Index = 0; Index < NumSimultaneousRenderTargets; ++Index)
		{
			Mask |= Bit;
			Bit <<= 1;
		}
		CurrentRTVOverlapMask = Mask;
		bTargetChanged = true;
	}

	// 실제로 무언가가 변경된 경우에만 렌더 타겟을 변경하는 D3D 호출을 수행합니다.
	if (bTargetChanged)
	{
		CommitRenderTargets(true);
		CurrentUAVMask = 0;
	}

	// 뷰포트를 렌더 타겟 0의 전체 크기로 설정합니다.
	if (NewRenderTargetViews[0])
	{
		// check target 0 is valid
		_ASSERT(0 < NewNumSimultaneousRenderTargets && NewRenderTargetsRHI[0].Texture != nullptr);
		FRTVDesc RTTDesc = GetRenderTargetViewDesc(NewRenderTargetViews[0]);
		RHISetViewport(0.0f, 0.0f, 0.0f, (float)RTTDesc.Width, (float)RTTDesc.Height, 1.0f);
	}
	else if (DepthStencilView)
	{
		TRefCountPtr<ID3D11Texture2D> DepthTargetTexture;
		DepthStencilView->GetResource((ID3D11Resource**)DepthTargetTexture.GetInitReference());

		D3D11_TEXTURE2D_DESC DTTDesc;
		DepthTargetTexture->GetDesc(&DTTDesc);
		RHISetViewport(0.0f, 0.0f, 0.0f, (float)DTTDesc.Width, (float)DTTDesc.Height, 1.0f);
	}
}

template<EShaderFrequency ShaderFrequency>
void FD3D11DynamicRHI::SetShaderParametersCommon(const TArray<FRHIShaderParameterResource>& InResourceParameters)
{
	for (const FRHIShaderParameterResource& Parameter : InResourceParameters)
	{
		switch (Parameter.Type)
		{
		case FRHIShaderParameterResource::EType::Texture:
		{
			FD3D11Texture* D3D11Texture = FD3D11DynamicRHI::ResourceCast(static_cast<FRHITexture*>(Parameter.Resource));
			ID3D11ShaderResourceView* ShaderResourceView = D3D11Texture ? D3D11Texture->GetShaderResourceView() : nullptr;
			uint8 Index = Parameter.Index;
			////Binder.SetTexture(static_cast<FRHITexture*>(Parameter.Resource), Parameter.Index);
			SetShaderResourceView<ShaderFrequency>(
				D3D11Texture,
				ShaderResourceView,
				Index
			);
			break;
		}
		//case FRHIShaderParameterResource::EType::ResourceView:
		//	//Binder.SetSRV(static_cast<FRHIShaderResourceView*>(Parameter.Resource), Parameter.Index);
		//	break;
		/*case FRHIShaderParameterResource::EType::UnorderedAccessView:
			break;*/
		case FRHIShaderParameterResource::EType::Sampler:
		{
			ID3D11SamplerState* Sampler = FD3D11DynamicRHI::ResourceCast(static_cast<FRHISamplerState*>(Parameter.Resource))->Resource;
			StateCache.SetSamplerState<ShaderFrequency>(Sampler, Parameter.Index);
			//Binder.SetSampler(static_cast<FRHISamplerState*>(Parameter.Resource), Parameter.Index);
			break;
		}
		//case FRHIShaderParameterResource::EType::UniformBuffer:
		//	//BindUniformBuffer<ShaderFrequency>(Parameter.Index, static_cast<FRHIUniformBuffer*>(Parameter.Resource));
		//	break;
		default:
			_ASSERT(false, TEXT("Unhandled resource type?"));
			break;
		}
	}
}

void FD3D11DynamicRHI::RHISetShaderParameters(FRHIGraphicsShader* Shader, TArray<FRHIShaderParameterResource>& InResourceParameters)
{
	switch (Shader->GetFrequency())
	{
	case SF_Vertex:
		//VALIDATE_BOUND_SHADER(static_cast<FRHIVertexShader*>(Shader));
		SetShaderParametersCommon<SF_Vertex>(InResourceParameters);
		break;
		//case SF_Geometry:
		//    //VALIDATE_BOUND_SHADER(static_cast<FRHIGeometryShader*>(Shader));
		//    //SetShaderParametersCommon<SF_Geometry>(nullptr/*GSConstantBuffer*/, InParametersData, InParameters, InResourceParameters);
		//    break;
	case SF_Pixel:
		//VALIDATE_BOUND_SHADER(static_cast<FRHIPixelShader*>(Shader));
		SetShaderParametersCommon<SF_Pixel>(InResourceParameters);
		break;
	default:
		_ASSERT(0, TEXT("Undefined FRHIGraphicsShader Type %d!"), (int32)Shader->GetFrequency());
	}
}