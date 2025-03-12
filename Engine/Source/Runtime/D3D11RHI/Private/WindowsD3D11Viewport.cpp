#include "D3D11Viewport.h"
#include "D3D11RHIPrivate.h"
#include "D3D11Util.h"

static DXGI_SWAP_EFFECT GSwapEffect = DXGI_SWAP_EFFECT_DISCARD;
static DXGI_SCALING GSwapScaling = DXGI_SCALING_STRETCH;
static uint32 GSwapChainBufferCount = 1;

static int32 GD3D11UseAllowTearing = 1;
uint32 FD3D11Viewport::GSwapChainFlags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

FD3D11Viewport::FD3D11Viewport(FD3D11DynamicRHI* InD3DRHI, HWND InWindowHandle, uint32 InSizeX, uint32 InSizeY, bool bInIsFullscreen, EPixelFormat InPreferredPixelFormat)
	: D3DRHI(InD3DRHI)
	, WindowHandle(InWindowHandle)
	, SizeX(InSizeX)
	, SizeY(InSizeY)
	, bIsFullscreen(bInIsFullscreen)
	, PixelFormat(InPreferredPixelFormat)
{
	// D3D 장치가 생성되었는지 확인하십시오.
	D3DRHI->InitD3DDevice();

	if (PixelFormat != EPixelFormat::PF_A2B10G10R10)
	{
		E_LOG(Error, TEXT("우리는 EPixelFormat::PF_A2B10G10R10만 지원"));
	}

	// 각 뷰포트에 대해 백버퍼/스왑체인을 생성합니다.
	/*TRefCountPtr<IDXGIDevice> DXGIDevice;
	VERIFYD3D11RESULT_EX(D3DRHI->GetDevice()->QueryInterface(IID_PPV_ARGS(DXGIDevice.GetInitReference())), D3DRHI->GetDevice());*/

	{
		GSwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		GSwapScaling = DXGI_SCALING_STRETCH;
		GSwapChainFlags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		IDXGIFactory1* Factory1 = D3DRHI->GetFactory();
		TRefCountPtr<IDXGIFactory5> Factory5;

		if (GD3D11UseAllowTearing)
		{
			if (S_OK == Factory1->QueryInterface(IID_PPV_ARGS(Factory5.GetInitReference())))
			{
				UINT AllowTearing = 0;
				if (S_OK == Factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &AllowTearing, sizeof(UINT)) && AllowTearing != 0)
				{
					GSwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
					GSwapScaling = DXGI_SCALING_NONE;
					GSwapChainFlags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
					bAllowTearing = true;
					GSwapChainBufferCount = 2;
				}
			}
		}
	}

	DXGI_FORMAT SwapChainFormat = GetRenderTargetFormat(PixelFormat);
	{
		TRefCountPtr<IDXGIFactory2> Factory2;
		const bool bSupportsFactory2 = SUCCEEDED(D3DRHI->GetFactory()->QueryInterface(__uuidof(IDXGIFactory2), (void**)Factory2.GetInitReference()));

		// Try and create a swapchain capable of being used on HDR monitors
		if ((SwapChain == nullptr) /*&& InD3DRHI->bDXGISupportsHDR*/ && bSupportsFactory2)
		{
			// Create the swapchain.
			DXGI_SWAP_CHAIN_DESC1 SwapChainDesc;
			ZeroMemory(&SwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC1));
			SwapChainDesc.Width = SizeX;
			SwapChainDesc.Height = SizeY;
			SwapChainDesc.SampleDesc.Count = 1;
			SwapChainDesc.SampleDesc.Quality = 0;
			SwapChainDesc.Format = SwapChainFormat;
			SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;

			DXGI_SWAP_CHAIN_FULLSCREEN_DESC FSSwapChainDesc = {};
			FSSwapChainDesc.Windowed = !bIsFullscreen;

			// Needed for HDR
			BackBufferCount = 2;
			SwapChainDesc.SwapEffect = GSwapEffect;
			SwapChainDesc.BufferCount = BackBufferCount;
			SwapChainDesc.Flags = GSwapChainFlags;
			SwapChainDesc.Scaling = GSwapScaling;

			IDXGISwapChain1* SwapChain1 = nullptr;
			HRESULT CreateSwapChainForHwndResult = Factory2->CreateSwapChainForHwnd(D3DRHI->GetDevice(), WindowHandle, &SwapChainDesc, &FSSwapChainDesc, nullptr, &SwapChain1);
			if (SUCCEEDED(CreateSwapChainForHwndResult))
			{
				SwapChain1->QueryInterface(IID_PPV_ARGS(SwapChain.GetInitReference()));

				//RECT WindowRect = {};
				//GetWindowRect(WindowHandle, &WindowRect);

				//FVector2D WindowTopLeft((float)WindowRect.left, (float)WindowRect.top);
				//FVector2D WindowBottomRight((float)WindowRect.right, (float)WindowRect.bottom);
				//bool bHDREnabled = false;
				//EDisplayColorGamut LocalDisplayColorGamut = DisplayColorGamut;
				//EDisplayOutputFormat LocalDisplayOutputFormat = DisplayOutputFormat;

				//HDRGetMetaData(LocalDisplayOutputFormat, LocalDisplayColorGamut, bHDREnabled, WindowTopLeft, WindowBottomRight, (void*)WindowHandle);
				//if (bHDREnabled)
				//{
				//	DisplayOutputFormat = LocalDisplayOutputFormat;
				//	DisplayColorGamut = LocalDisplayColorGamut;
				//}

				//// See if we are running on a HDR monitor 
				//CheckHDRMonitorStatus();
			}
			else
			{
				E_LOG(Log, TEXT("CreateSwapChainForHwnd failed with result '{}' (0x{:X}), falling back to legacy CreateSwapChain."),
					GetD3D11ErrorString(CreateSwapChainForHwndResult, D3DRHI->GetDevice()),
					CreateSwapChainForHwndResult);
			}
		}

		// DXGI 메시지 후크를 설정하여 우리의 뒤에서 윈도우가 변경되지 않도록 합니다.
		D3DRHI->GetFactory()->MakeWindowAssociation(WindowHandle, DXGI_MWA_NO_WINDOW_CHANGES);
	}

	// 뷰포트의 백 버퍼를 나타내는 RHI 서피스를 생성합니다.
	BackBuffer = GetSwapChainSurface(D3DRHI, PixelFormat, SizeX, SizeY, SwapChain);
}

FD3D11Viewport::~FD3D11Viewport()
{
	SwapChain->Release();
}

D3D11RHI_API FD3D11Texture* FD3D11Viewport::GetSwapChainSurface(FD3D11DynamicRHI* D3DRHI, EPixelFormat PixelFormat, uint32 SizeX, uint32 SizeY, IDXGISwapChain* SwapChain)
{
	// 백 버퍼를 가져옵니다.
	TRefCountPtr<ID3D11Texture2D> BackBufferResource;
	if (SwapChain)
	{
		VERIFYD3D11RESULT_EX(SwapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)BackBufferResource.GetInitReference()), D3DRHI->GetDevice());
	}

	// 렌더 타겟 뷰를 생성합니다.
	TRefCountPtr<ID3D11RenderTargetView> BackBufferRenderTargetView;
	D3D11_RENDER_TARGET_VIEW_DESC RTVDesc;
	RTVDesc.Format = DXGI_FORMAT_UNKNOWN;
	RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	RTVDesc.Texture2D.MipSlice = 0;
	VERIFYD3D11RESULT_EX(D3DRHI->GetDevice()->CreateRenderTargetView(BackBufferResource, &RTVDesc, BackBufferRenderTargetView.GetInitReference()), D3DRHI->GetDevice());
	
	D3D11_TEXTURE2D_DESC TextureDesc;
	BackBufferResource->GetDesc(&TextureDesc);

	TArray<TRefCountPtr<ID3D11RenderTargetView> > RenderTargetViews;
	RenderTargetViews.push_back(BackBufferRenderTargetView);

	// 백 버퍼를 텍스처로 사용하기 위해 셰이더 리소스 뷰를 생성합니다.
	TRefCountPtr<ID3D11ShaderResourceView> BackBufferShaderResourceView;
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MostDetailedMip = 0;
	SRVDesc.Texture2D.MipLevels = 1;
	VERIFYD3D11RESULT_EX(D3DRHI->GetDevice()->CreateShaderResourceView(BackBufferResource, &SRVDesc, BackBufferShaderResourceView.GetInitReference()), D3DRHI->GetDevice());

	FRHITextureCreateDesc CreateDesc =
		FRHITextureCreateDesc::Create2D(TEXT("FD3D11Viewport::GetSwapChainSurface"), TextureDesc.Width, TextureDesc.Height, PixelFormat)
		.SetFlags(ETextureCreateFlags::RenderTargetable)
		.DetermineInititialState();
	CreateDesc.SetClearValue(FClearValueBinding(FLinearColor::Black));

	FD3D11Texture* NewTexture = new FD3D11Texture(
		CreateDesc,
		BackBufferResource,
		BackBufferShaderResourceView,
		1,
		false,
		RenderTargetViews,
		{}
	);

	return NewTexture;
}

void FD3D11Viewport::Resize(const uint32 NewSizeX, const uint32 NewSizeY)
{
	E_LOG(Log, TEXT("Request Resize : {}, {}"), NewSizeX, NewSizeY);
	_ASSERT(NewSizeX > 0);
	_ASSERT(NewSizeY > 0);
	SizeX = NewSizeX;
	SizeY = NewSizeY;
	D3DRHI->SetRenderTargets(0, nullptr, nullptr);
	D3DRHI->ClearState();
	BackBuffer.SafeRelease();

	DXGI_SWAP_CHAIN_DESC Desc;
	HRESULT Hr = SwapChain->GetDesc(&Desc);
	if (FAILED(Hr))
	{
		E_LOG(Error, TEXT("FD3D11Viewport::Resize - SwapChain->GetDesc {:#x}"), Hr);
		return;
	}

	Hr = SwapChain->ResizeBuffers(Desc.BufferCount, SizeX, SizeY, Desc.BufferDesc.Format, Desc.Flags);
	if (FAILED(Hr))
	{
		E_LOG(Error, TEXT("FD3D11Viewport::Resize - SwapChain->ResizeBuffer {:#x}"), Hr);
		return;
	}

	BackBuffer = GetSwapChainSurface(D3DRHI, PixelFormat, SizeX, SizeY, SwapChain);
}

D3D11RHI_API bool FD3D11Viewport::Present(bool bLockToVsync)
{
	HRESULT Result = S_OK;
	if (ValidState != 0 && SwapChain)
	{
		// Check if the viewport's swap chain has been invalidated by DXGI.
		BOOL bSwapChainFullscreenState;
		TRefCountPtr<IDXGIOutput> SwapChainOutput;
		VERIFYD3D11RESULT_EX(SwapChain->GetFullscreenState(&bSwapChainFullscreenState, SwapChainOutput.GetInitReference()), D3DRHI->GetDevice());
		// Can't compare BOOL with bool...
		if ((!!bSwapChainFullscreenState) != bIsFullscreen)
		{
			ValidState = VIEWPORT_INVALID;
		}
	}
	if (MaximumFrameLatency != 3/*RHIConsoleVariables::MaximumFrameLatency*/)
	{
		MaximumFrameLatency = 3;// RHIConsoleVariables::MaximumFrameLatency;
		TRefCountPtr<IDXGIDevice1> DXGIDevice;
		VERIFYD3D11RESULT_EX(D3DRHI->GetDevice()->QueryInterface(IID_IDXGIDevice, (void**)DXGIDevice.GetInitReference()), D3DRHI->GetDevice());
		DXGIDevice->SetMaximumFrameLatency(MaximumFrameLatency);
	}
	if (0 != (ValidState & VIEWPORT_INVALID))
	{
		return false;
	}

	if (SwapChain)
	{
		// Check if the viewport's swap chain has been invalidated by DXGI.
		BOOL bSwapChainFullscreenState;
		TRefCountPtr<IDXGIOutput> SwapChainOutput;
		SwapChain->GetFullscreenState(&bSwapChainFullscreenState, SwapChainOutput.GetInitReference());
		// Can't compare BOOL with bool...
		if ((!!bSwapChainFullscreenState) != bIsFullscreen)
		{
			ValidState = (VIEWPORT_INVALID | VIEWPORT_FULLSCREEN_LOST);
		}
		else
		{
			// Present the back buffer to the viewport window.
			uint32 Flags = 0;
			if ((GetSwapChainFlags() & DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING) != 0 && !bLockToVsync /*!SyncInterval*/ && !bIsFullscreen)
			{
				Flags |= DXGI_PRESENT_ALLOW_TEARING;
			}
			Result = SwapChain->Present(bLockToVsync/*SyncInterval*/, Flags);
		}
	}

	D3DRHI->GetDeviceContext()->OMSetRenderTargets(0, 0, 0);

	return true;
}

D3D11RHI_API uint32 FD3D11Viewport::GetSwapChainFlags()
{
	uint32 SwapChainFlags = GSwapChainFlags;

	// AllowTearing의 일관성을 보장하지 않으면 ResizeBuffers가 E_INVALIDARG 오류와 함께 실패합니다.
	if (bAllowTearing != !!(SwapChainFlags & DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING))
	{
		SwapChainFlags ^= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
	}

	return SwapChainFlags;
}
