#pragma once
#include "CoreMinimal.h"
#include "SceneUtils.h"
#include "SceneTextures.h"

class FViewFamilyInfo : public FSceneViewFamily
{
public:
	explicit FViewFamilyInfo(const FSceneViewFamily& InViewFamily);
	virtual ~FViewFamilyInfo() {}

	EShadingPath GetShadingPath() const { return ShadingPath; }

	/** Get scene textures associated with this view family -- asserts or checks that they have been initialized */
	inline FSceneTextures& GetSceneTextures()
	{
		_ASSERT(bIsSceneTexturesInitialized, TEXT("FSceneTextures was not initialized. Call FSceneTextures::InitializeViewFamily() first."));
		return SceneTextures;
	}

	inline const FSceneTextures& GetSceneTextures() const
	{
		_ASSERT(bIsSceneTexturesInitialized, TEXT("FSceneTextures was not initialized. Call FSceneTextures::InitializeViewFamily() first."));
		return SceneTextures;
	}

	inline FSceneTextures* GetSceneTexturesChecked()
	{
		return bIsSceneTexturesInitialized ? &SceneTextures : nullptr;
	}

	inline const FSceneTextures* GetSceneTexturesChecked() const
	{
		return bIsSceneTexturesInitialized ? &SceneTextures : nullptr;
	}

private:
	friend struct FMinimalSceneTextures;
	friend struct FSceneTextures;

	FSceneTextures SceneTextures;

	EShadingPath ShadingPath = EShadingPath::Num;
};

/**
 * 장면 렌더링 함수들의 범위로 사용됩니다.
 * FSceneViewFamily::BeginRender에 의해 게임 스레드에서 초기화되고, 렌더링 스레드로 전달됩니다.
 * 렌더링 스레드는 Render()를 호출하고, 반환되면 장면 렌더러를 삭제합니다.
 */
class FSceneRenderer
{
public:
	FSceneRenderer(const FSceneViewFamily* InViewFamily/*, FHitProxyConsumer* HitProxyConsumer*/);
	virtual ~FSceneRenderer();

	virtual void Render();

	FORCEINLINE FSceneTextures& GetActiveSceneTextures() { return ViewFamily.GetSceneTextures(); }

protected:
	void RenderLight();
	void RenderMesh();

	void RenderDeferredLight();

protected:
	void DrawRectangle(FRHIVertexShader* VertexShader, FRHIPixelShader* PixelShader);

protected:
	/** 렌더링 중인 뷰 패밀리. 이 변수는 Views 배열을 참조합니다. */
	FViewFamilyInfo ViewFamily;

	struct MS_ALIGN(SHADER_PARAMETER_STRUCT_ALIGNMENT) FSceneUniformBuffer
	{
		FVector3D EyePosition;
		int NumRadianceMipLevels;
		FMatrix ViewMatrix = FMatrix::Identity;
		FMatrix ProjectionMatrix = FMatrix::Identity;
		FMatrix ViewProjectionMatrix = FMatrix::Identity;
	};
	FSceneUniformBuffer SceneUniformBuffer;
	FUniformBufferRHIRef SceneUniformBufferRHI;

	struct MS_ALIGN(SHADER_PARAMETER_STRUCT_ALIGNMENT) FXAAConstants
	{
		FVector2D ScreenSize;
	};
	FXAAConstants FXAAUniformBuffer;
	FUniformBufferRHIRef FXAAUniformBufferRHI;
};