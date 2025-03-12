#include "D3D11RHIPrivate.h"
#include "RenderResource.h"

IMPLEMENT_MODULE(FD3D11DynamicRHIModule, D3D11RHI);

uint32 FD3D11DynamicRHI::GetMaxMSAAQuality(uint32 SampleCount)
{
	if (SampleCount <= DX_MAX_MSAA_COUNT)
	{
		// 0 has better quality (a more even distribution)
		// higher quality levels might be useful for non box filtered AA or when using weighted samples 
		return 0;
		//		return AvailableMSAAQualities[SampleCount];
	}
	// not supported
	return 0xffffffff;
}

FD3D11DynamicRHI::FD3D11DynamicRHI(IDXGIFactory1* InDXGIFactory1, D3D_FEATURE_LEVEL InFeatureLevel, const FD3D11Adapter& InAdapter)
	: DXGIFactory1(InDXGIFactory1)
	, FeatureLevel(InFeatureLevel)
	, Adapter(InAdapter)
{

	// Initialize the platform pixel format map.
	GPixelFormats[PF_Unknown].PlatformFormat = DXGI_FORMAT_UNKNOWN;
	GPixelFormats[PF_A32B32G32R32F].PlatformFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
	GPixelFormats[PF_B8G8R8A8].PlatformFormat = DXGI_FORMAT_B8G8R8A8_TYPELESS;
	GPixelFormats[PF_G8].PlatformFormat = DXGI_FORMAT_R8_UNORM;
	GPixelFormats[PF_G16].PlatformFormat = DXGI_FORMAT_R16_UNORM;
	GPixelFormats[PF_DXT1].PlatformFormat = DXGI_FORMAT_BC1_TYPELESS;
	GPixelFormats[PF_DXT3].PlatformFormat = DXGI_FORMAT_BC2_TYPELESS;
	GPixelFormats[PF_DXT5].PlatformFormat = DXGI_FORMAT_BC3_TYPELESS;
	GPixelFormats[PF_BC4].PlatformFormat = DXGI_FORMAT_BC4_UNORM;
	GPixelFormats[PF_UYVY].PlatformFormat = DXGI_FORMAT_UNKNOWN;		// TODO: Not supported in D3D11
	/*if (CVarD3D11UseD24.GetValueOnAnyThread())
	{
		GPixelFormats[PF_DepthStencil].PlatformFormat = DXGI_FORMAT_R24G8_TYPELESS;
		GPixelFormats[PF_DepthStencil].BlockBytes = 4;
		GPixelFormats[PF_DepthStencil].bIs24BitUnormDepthStencil = true;
		GPixelFormats[PF_X24_G8].PlatformFormat = DXGI_FORMAT_X24_TYPELESS_G8_UINT;
		GPixelFormats[PF_X24_G8].BlockBytes = 4;
	}
	else*/
	{
		GPixelFormats[PF_DepthStencil].PlatformFormat = DXGI_FORMAT_R32G8X24_TYPELESS;
		GPixelFormats[PF_DepthStencil].BlockBytes = 5;
		GPixelFormats[PF_DepthStencil].bIs24BitUnormDepthStencil = false;
		GPixelFormats[PF_X24_G8].PlatformFormat = DXGI_FORMAT_X32_TYPELESS_G8X24_UINT;
		GPixelFormats[PF_X24_G8].BlockBytes = 5;
	}
	GPixelFormats[PF_DepthStencil].Supported = true;
	GPixelFormats[PF_X24_G8].Supported = true;
	GPixelFormats[PF_ShadowDepth].PlatformFormat = DXGI_FORMAT_R16_TYPELESS;
	GPixelFormats[PF_ShadowDepth].BlockBytes = 2;
	GPixelFormats[PF_ShadowDepth].Supported = true;
	GPixelFormats[PF_R32_FLOAT].PlatformFormat = DXGI_FORMAT_R32_FLOAT;
	GPixelFormats[PF_G16R16].PlatformFormat = DXGI_FORMAT_R16G16_UNORM;
	GPixelFormats[PF_G16R16F].PlatformFormat = DXGI_FORMAT_R16G16_FLOAT;
	GPixelFormats[PF_G16R16F_FILTER].PlatformFormat = DXGI_FORMAT_R16G16_FLOAT;
	GPixelFormats[PF_G32R32F].PlatformFormat = DXGI_FORMAT_R32G32_FLOAT;
	GPixelFormats[PF_A2B10G10R10].PlatformFormat = DXGI_FORMAT_R10G10B10A2_UNORM;
	GPixelFormats[PF_A16B16G16R16].PlatformFormat = DXGI_FORMAT_R16G16B16A16_UNORM;
	GPixelFormats[PF_D24].PlatformFormat = DXGI_FORMAT_R24G8_TYPELESS;
	GPixelFormats[PF_R16F].PlatformFormat = DXGI_FORMAT_R16_FLOAT;
	GPixelFormats[PF_R16F_FILTER].PlatformFormat = DXGI_FORMAT_R16_FLOAT;

	GPixelFormats[PF_FloatRGB].PlatformFormat = DXGI_FORMAT_R11G11B10_FLOAT;
	GPixelFormats[PF_FloatRGB].BlockBytes = 4;
	GPixelFormats[PF_FloatRGBA].PlatformFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
	GPixelFormats[PF_FloatRGBA].BlockBytes = 8;

	GPixelFormats[PF_FloatR11G11B10].PlatformFormat = DXGI_FORMAT_R11G11B10_FLOAT;
	GPixelFormats[PF_FloatR11G11B10].BlockBytes = 4;
	GPixelFormats[PF_FloatR11G11B10].Supported = true;

	GPixelFormats[PF_V8U8].PlatformFormat = DXGI_FORMAT_R8G8_SNORM;
	GPixelFormats[PF_BC5].PlatformFormat = DXGI_FORMAT_BC5_UNORM;
	GPixelFormats[PF_A1].PlatformFormat = DXGI_FORMAT_R1_UNORM; // Not supported for rendering.
	GPixelFormats[PF_A8].PlatformFormat = DXGI_FORMAT_A8_UNORM;
	GPixelFormats[PF_R32_UINT].PlatformFormat = DXGI_FORMAT_R32_UINT;
	GPixelFormats[PF_R32_SINT].PlatformFormat = DXGI_FORMAT_R32_SINT;

	GPixelFormats[PF_R16_UINT].PlatformFormat = DXGI_FORMAT_R16_UINT;
	GPixelFormats[PF_R16_SINT].PlatformFormat = DXGI_FORMAT_R16_SINT;
	GPixelFormats[PF_R16G16B16A16_UINT].PlatformFormat = DXGI_FORMAT_R16G16B16A16_UINT;
	GPixelFormats[PF_R16G16B16A16_SINT].PlatformFormat = DXGI_FORMAT_R16G16B16A16_SINT;

	GPixelFormats[PF_R5G6B5_UNORM].PlatformFormat = DXGI_FORMAT_B5G6R5_UNORM;
	GPixelFormats[PF_R5G6B5_UNORM].Supported = true;
	GPixelFormats[PF_B5G5R5A1_UNORM].PlatformFormat = DXGI_FORMAT_B5G5R5A1_UNORM;
	GPixelFormats[PF_B5G5R5A1_UNORM].Supported = true;
	GPixelFormats[PF_R8G8B8A8].PlatformFormat = DXGI_FORMAT_R8G8B8A8_TYPELESS;
	GPixelFormats[PF_R8G8B8A8_UINT].PlatformFormat = DXGI_FORMAT_R8G8B8A8_UINT;
	GPixelFormats[PF_R8G8B8A8_SNORM].PlatformFormat = DXGI_FORMAT_R8G8B8A8_SNORM;
	GPixelFormats[PF_R8G8].PlatformFormat = DXGI_FORMAT_R8G8_UNORM;
	GPixelFormats[PF_R32G32B32A32_UINT].PlatformFormat = DXGI_FORMAT_R32G32B32A32_UINT;
	GPixelFormats[PF_R16G16_UINT].PlatformFormat = DXGI_FORMAT_R16G16_UINT;
	GPixelFormats[PF_R32G32_UINT].PlatformFormat = DXGI_FORMAT_R32G32_UINT;

	GPixelFormats[PF_BC6H].PlatformFormat = DXGI_FORMAT_BC6H_UF16;
	GPixelFormats[PF_BC7].PlatformFormat = DXGI_FORMAT_BC7_TYPELESS;
	GPixelFormats[PF_R8_UINT].PlatformFormat = DXGI_FORMAT_R8_UINT;
	GPixelFormats[PF_R8].PlatformFormat = DXGI_FORMAT_R8_UNORM;

	GPixelFormats[PF_R16G16B16A16_UNORM].PlatformFormat = DXGI_FORMAT_R16G16B16A16_UNORM;
	GPixelFormats[PF_R16G16B16A16_SNORM].PlatformFormat = DXGI_FORMAT_R16G16B16A16_SNORM;

	GPixelFormats[PF_NV12].PlatformFormat = DXGI_FORMAT_NV12;
	GPixelFormats[PF_NV12].Supported = true;

	GPixelFormats[PF_G16R16_SNORM].PlatformFormat = DXGI_FORMAT_R16G16_SNORM;
	GPixelFormats[PF_R8G8_UINT].PlatformFormat = DXGI_FORMAT_R8G8_UINT;
	GPixelFormats[PF_R32G32B32_UINT].PlatformFormat = DXGI_FORMAT_R32G32B32_UINT;
	GPixelFormats[PF_R32G32B32_SINT].PlatformFormat = DXGI_FORMAT_R32G32B32_SINT;
	GPixelFormats[PF_R32G32B32F].PlatformFormat = DXGI_FORMAT_R32G32B32_FLOAT;
	GPixelFormats[PF_R8_SINT].PlatformFormat = DXGI_FORMAT_R8_SINT;

	GPixelFormats[PF_P010].PlatformFormat = DXGI_FORMAT_P010;
	GPixelFormats[PF_P010].Supported = true;
}

extern bool GIsDebugLayerEnabled;
extern D3D11RHI_API map<type_index, TRefCountPtr<FRHIShader>> RHIShaders;
extern unordered_map<FString, FUniformBufferRHIRef> UniformBuffers;
extern unordered_map<FString, FTextureRHIRef> Textures;

void FD3D11DynamicRHI::CleanupD3DDevice()
{
	UniformBuffers.clear();
	Textures.clear();
	RHIShaders.clear();
	E_LOG(Log, TEXT("CleanupD3DDevice"));
	_ASSERT(Direct3DDevice);
	_ASSERT(Direct3DDeviceIMContext);

	// Ask all initialized FRenderResources to release their RHI resources.
	FRenderResource::ReleaseRHIForAllResources();

	StateCache.SetContext(nullptr);

	Direct3DDeviceIMContext->ClearState();
	Direct3DDeviceIMContext->Flush();
	Direct3DDeviceIMContext = nullptr;

	if (GIsDebugLayerEnabled)
	{
		// Perform a detailed live object report (with resource types)
		ID3D11Debug* D3D11Debug;
		Direct3DDevice->QueryInterface(__uuidof(ID3D11Debug), (void**)(&D3D11Debug));
		if (D3D11Debug)
		{
			D3D11Debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL | D3D11_RLDO_IGNORE_INTERNAL);
		}
	}

	if (ExceptionHandlerHandle != INVALID_HANDLE_VALUE)
	{
		RemoveVectoredExceptionHandler(ExceptionHandlerHandle);
	}

	Direct3DDevice = nullptr;
}

void FD3D11DynamicRHI::Shutdown()
{
	CleanupD3DDevice();
}
