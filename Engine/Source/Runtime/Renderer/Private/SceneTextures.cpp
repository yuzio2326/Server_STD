#include "SceneTextures.h"
#include "SceneRendering.h"

RENDERER_API void FMinimalSceneTextures::InitializeViewFamily(FViewFamilyInfo& ViewFamily)
{
	const FVector2D& Extents = ViewFamily.ViewportSize;
	FSceneTextures& SceneTextures = ViewFamily.SceneTextures;

	{
		FRHITextureCreateDesc DepthDesc =
			FRHITextureCreateDesc::Create2D(TEXT("SceneDepthZ"),
				Extents.x, Extents.y, PF_DepthStencil)
			.SetFlags(ETextureCreateFlags::DepthStencilTargetable | ETextureCreateFlags::ShaderResource | ETextureCreateFlags::InputAttachmentRead)
			.SetClearValue(FClearValueBinding(0.f, 0)) // Dx 기본은 근평면 0.f / 원평면 1.f이라 1로 초기화 하는것이 일반적이지만, UE에서는 반대
			.DetermineInititialState();
		SceneTextures.Depth = GetCommandList().CreateTexture(DepthDesc);

		FRHITextureCreateDesc ColorDesc =
			FRHITextureCreateDesc::Create2D(TEXT("SceneColor"),
				Extents.x, Extents.y, PF_A2B10G10R10)// PF_FloatRGBA)
			.SetFlags(ETextureCreateFlags::RenderTargetable | ETextureCreateFlags::ShaderResource /*| ETextureCreateFlags::UAV*/)
			.SetClearValue(FClearValueBinding(FLinearColor(0.f, 0.f, 0.f, 1.f)))
			.DetermineInititialState();
		SceneTextures.Color = GetCommandList().CreateTexture(ColorDesc);
	}

	ViewFamily.bIsSceneTexturesInitialized = true;
}

RENDERER_API void FSceneTextures::InitializeViewFamily(FViewFamilyInfo& ViewFamily)
{
	FMinimalSceneTextures::InitializeViewFamily(ViewFamily);

	if (GetShadingPath() == EShadingPath::Deferred)
	{
		const FVector2D& Extents = ViewFamily.ViewportSize;
		FSceneTextures& SceneTextures = ViewFamily.SceneTextures;

		// GBufferA: Normal
		{
			FRHITextureCreateDesc Desc =
				FRHITextureCreateDesc::Create2D(TEXT("GBufferA"), Extents.x, Extents.y, PF_A32B32G32R32F)
				.SetFlags(ETextureCreateFlags::RenderTargetable | ETextureCreateFlags::ShaderResource)
				.SetClearValue(FClearValueBinding(FLinearColor(0.f, 0.f, 0.f, 0.f)));
			SceneTextures.GBufferA = GetCommandList().CreateTexture(Desc);
		}
		// GBufferB: Albedo(=BaseColor, RGB), Specular(A: 우리는 Specular 없음)
		{
			FRHITextureCreateDesc Desc =
				FRHITextureCreateDesc::Create2D(TEXT("GBufferB"), Extents.x, Extents.y, PF_B8G8R8A8)
				.SetFlags(ETextureCreateFlags::RenderTargetable | ETextureCreateFlags::ShaderResource)
				.SetClearValue(FClearValueBinding(FLinearColor(0.f, 0.f, 0.f, 0.f)));
			SceneTextures.GBufferB = GetCommandList().CreateTexture(Desc);
		}
		// GBufferC: Metallic(R), Roughness(G), Ambient Occlusion(B)
		{
			FRHITextureCreateDesc Desc =
				FRHITextureCreateDesc::Create2D(TEXT("GBufferC"), Extents.x, Extents.y, PF_B8G8R8A8)
				.SetFlags(ETextureCreateFlags::RenderTargetable | ETextureCreateFlags::ShaderResource)
				.SetClearValue(FClearValueBinding(FLinearColor(0.f, 0.f, 0.f, 0.f)));
			SceneTextures.GBufferC = GetCommandList().CreateTexture(Desc);
		}
		// GBufferD: World Position(RGB)
		{
			FRHITextureCreateDesc Desc =
				FRHITextureCreateDesc::Create2D(TEXT("GBufferD"), Extents.x, Extents.y, PF_A32B32G32R32F)
				.SetFlags(ETextureCreateFlags::RenderTargetable | ETextureCreateFlags::ShaderResource)
				.SetClearValue(FClearValueBinding(FLinearColor(0.f, 0.f, 0.f, 0.f)));
			SceneTextures.GBufferD = GetCommandList().CreateTexture(Desc);
		}
	}
}

RENDERER_API EPixelFormat FSceneTextures::GetGBufferFFormatAndCreateFlags(ETextureCreateFlags& OutCreateFlags)
{
	_ASSERT(false);
	return EPixelFormat();
}
