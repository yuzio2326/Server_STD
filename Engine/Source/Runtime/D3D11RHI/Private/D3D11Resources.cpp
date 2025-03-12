#include "D3D11Resources.h"

FD3D11Texture::FD3D11Texture(const FRHITextureCreateDesc& InDesc, ID3D11Resource* InResource, ID3D11ShaderResourceView* InShaderResourceView, int32 InRTVArraySize, bool bInCreatedRTVsPerSlice, TArray<TRefCountPtr<ID3D11RenderTargetView>> InRenderTargetViews, TArray<TRefCountPtr<ID3D11DepthStencilView>> InDepthStencilViews)
	: FRHITexture(InDesc)
	, Resource(InResource)
	, ShaderResourceView(InShaderResourceView)
	, RenderTargetViews(InRenderTargetViews)
	, RTVArraySize(InRTVArraySize)
	, bCreatedRTVsPerSlice(bInCreatedRTVsPerSlice)
	, bAlias(false)
{
	// 모든 액세스 유형 조합에 대한 DSV(Depth Stencil View)를 설정합니다.
	if (InDepthStencilViews.size())
	{
		_ASSERT(InDepthStencilViews.size() == FExclusiveDepthStencil::MaxIndex);
		for (uint32 Index = 0; Index < FExclusiveDepthStencil::MaxIndex; Index++)
		{
			DepthStencilViews[Index] = InDepthStencilViews[Index];
		}
	}
}

FD3D11Texture::FD3D11Texture(FD3D11Texture const& Other, const FString& Name, EAliasResourceParam)
	: FRHITexture(FRHITextureCreateDesc(Other.GetDesc(), ERHIAccess::SRVMask, Name.data()))
	, bAlias(true)
{
	AliasResource(Other);
}

D3D11RHI_API void FD3D11Texture::AliasResource(FD3D11Texture const& Other)
{
	_ASSERT(bAlias);
	IHVResourceHandle = Other.IHVResourceHandle;
	Resource = Other.Resource;
	ShaderResourceView = Other.ShaderResourceView;
	RenderTargetViews = Other.RenderTargetViews;
	bCreatedRTVsPerSlice = Other.bCreatedRTVsPerSlice;
	RTVArraySize = Other.RTVArraySize;

	for (int32 Index = 0; Index < ARRAYSIZE(DepthStencilViews); ++Index)
	{
		DepthStencilViews[Index] = Other.DepthStencilViews[Index];
	}
}

FD3D11Texture::~FD3D11Texture()
{
}

void FD3D11ViewableResource::UpdateLinkedViews()
{
	_ASSERT(false);
}
