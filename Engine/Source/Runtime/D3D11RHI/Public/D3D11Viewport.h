#pragma once
#include "CoreMinimal.h"
#include "D3D11Resources.h"

class FD3D11Viewport : public FRHIViewport
{
public:
	enum ED3DViewportValidFlags : uint32
	{
		VIEWPORT_INVALID = 0x1,
		VIEWPORT_FULLSCREEN_LOST = 0x2,
	};

	D3D11RHI_API FD3D11Viewport(class FD3D11DynamicRHI* InD3DRHI, HWND InWindowHandle, uint32 InSizeX, uint32 InSizeY, bool bInIsFullscreen, EPixelFormat InPreferredPixelFormat);
	D3D11RHI_API ~FD3D11Viewport();

	static D3D11RHI_API FD3D11Texture* GetSwapChainSurface(FD3D11DynamicRHI* D3DRHI, EPixelFormat PixelFormat, uint32 SizeX, uint32 SizeY, IDXGISwapChain* SwapChain);

	static DXGI_FORMAT GetRenderTargetFormat(EPixelFormat PixelFormat)
	{
		DXGI_FORMAT	DXFormat = (DXGI_FORMAT)GPixelFormats[PixelFormat].PlatformFormat;
		switch (DXFormat)
		{
		case DXGI_FORMAT_B8G8R8A8_TYPELESS:		return DXGI_FORMAT_B8G8R8A8_UNORM;
		case DXGI_FORMAT_BC1_TYPELESS:			return DXGI_FORMAT_BC1_UNORM;
		case DXGI_FORMAT_BC2_TYPELESS:			return DXGI_FORMAT_BC2_UNORM;
		case DXGI_FORMAT_BC3_TYPELESS:			return DXGI_FORMAT_BC3_UNORM;
		case DXGI_FORMAT_R16_TYPELESS:			return DXGI_FORMAT_R16_UNORM;
		case DXGI_FORMAT_R8G8B8A8_TYPELESS:		return DXGI_FORMAT_R8G8B8A8_UNORM;
		default: 								return DXFormat;
		}
	}

	virtual void Resize(const uint32 NewSizeX, const uint32 NewSizeY);
	virtual FRHITexture* GetRenderTarget() const override { return BackBuffer; }

	/** 스왑 체인을 표시합니다.
	* 엔진에 의해 Present가 수행된 경우 true를 반환합니다.
	*/
	D3D11RHI_API bool Present(bool bLockToVsync);

	// Accessors.
	FVector3D GetSizeXY() const { return FVector3D(SizeX, SizeY, 0.f); }
	FD3D11Texture* GetBackBuffer() const { return BackBuffer; }

protected:
	/** Gets the swap chain flags */
	D3D11RHI_API uint32 GetSwapChainFlags();

protected:
	FD3D11DynamicRHI* D3DRHI = nullptr;
	HWND WindowHandle = NULL;
	uint32 MaximumFrameLatency = 0;
	uint32 SizeX = 0;
	uint32 SizeY = 0;
	uint32 BackBufferCount;
	uint32 ValidState = 0;
	bool bIsFullscreen = false;
	bool bAllowTearing = false;
	EPixelFormat PixelFormat;

	static D3D11RHI_API uint32 GSwapChainFlags;

	TRefCountPtr<IDXGISwapChain> SwapChain;
	TRefCountPtr<FD3D11Texture> BackBuffer;
};

template<>
struct TD3D11ResourceTraits<FRHIViewport>
{
	typedef FD3D11Viewport TConcreteType;
};