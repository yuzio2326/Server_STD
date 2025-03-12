#pragma once

#include "SceneView.h"

struct FSceneTextures;
//class FViewInfo;
class FViewFamilyInfo;

/** RDG 구조체로, 모든 렌더링 구성에 걸쳐 공통으로 사용되는 최소한의 씬 텍스처를 포함합니다. */
struct FMinimalSceneTextures
{
    // FViewFamilyInfo에서 최소한의 씬 텍스처 구조체를 초기화합니다.
    static RENDERER_API void InitializeViewFamily(/*FRDGBuilder& GraphBuilder,*/ FViewFamilyInfo& ViewFamily);

    // 씬 텍스처를 생성하는 데 사용된 설정의 변경 불가능한 사본입니다.
    //FSceneTexturesConfig Config;

    // 지연 렌더링 또는 모바일용 유니폼 버퍼입니다.
    //TRDGUniformBufferRef<FSceneTextureUniformParameters> UniformBuffer{};
    //TRDGUniformBufferRef<FMobileSceneTextureUniformParameters> MobileUniformBuffer{};

    // 유니폼 버퍼를 생성할 때 사용되는 설정 모드입니다. 이들은 필요에 따라 업데이트됩니다.
    //ESceneTextureSetupMode SetupMode = ESceneTextureSetupMode::None;
    //EMobileSceneTextureSetupMode MobileSetupMode = EMobileSceneTextureSetupMode::None;

    // 라이팅이 포함된 씬 컬러 정보를 담은 텍스처로, 포스트 프로세싱이 적용되지 않은 상태입니다. MSAA가 적용된 경우 두 개의 텍스처가 존재합니다.
    FRDGTextureMSAA Color{};

    // 씬 깊이를 포함하는 텍스처입니다. MSAA가 적용된 경우 두 개의 텍스처가 존재합니다.
    FRDGTextureMSAA Depth{};

    // 해상된(만약 MSAA가 적용된 경우) 씬 깊이의 스텐실 뷰를 포함하는 텍스처입니다.
    //FRDGTextureSRVRef Stencil{};

    // 2차 깊이 패스에서 다른 메쉬가 렌더링되기 전에 기본 깊이 버퍼가 복사된 텍스처입니다.
    //FRDGTextureMSAA PartialDepth{};

    // 커스텀 깊이 패스에서 깊이/스텐실 정보를 포함하는 텍스처입니다.
    //FCustomDepthTextures CustomDepth{};

    // 제공된 FeatureLevel에 따른 씬 텍스처 셰이더 파라미터를 반환합니다.
    //RENDERER_API FSceneTextureShaderParameters GetSceneTextureShaderParameters(ERHIFeatureLevel::Type FeatureLevel) const;
};

/** 지연 렌더러 또는 모바일 렌더러를 위한 전체 씬 텍스처 세트를 포함하는 RDG 구조체입니다. */
struct FSceneTextures : public FMinimalSceneTextures
{
    // FViewFamilyInfo에서 씬 텍스처 구조체를 초기화합니다.
    static RENDERER_API void InitializeViewFamily(/*FRDGBuilder& GraphBuilder,*/ FViewFamilyInfo& ViewFamily);
    static RENDERER_API EPixelFormat GetGBufferFFormatAndCreateFlags(ETextureCreateFlags& OutCreateFlags);

    // GBuffer 패스를 위한 렌더 타겟 배열을 구성합니다.
//    RENDERER_API uint32 GetGBufferRenderTargets(
//        TArrayView<FTextureRenderTargetBinding> RenderTargets,
//        EGBufferLayout Layout = GBL_Default) const;
//    RENDERER_API uint32 GetGBufferRenderTargets(
//        ERenderTargetLoadAction LoadAction,
//        FRenderTargetBindingSlots& RenderTargets,
//        EGBufferLayout Layout = GBL_Default) const;
//
//    // (지연) 차폐를 위한 보수적으로 다운샘플된 깊이를 포함하는 텍스처입니다.
//    FRDGTextureRef SmallDepth{};
//
    // (지연) 지연 셰이딩을 위한 기하 정보를 포함하는 텍스처들입니다.
    FTextureRHIRef GBufferA{}; // Normal
    FTextureRHIRef GBufferB{}; // Albedo(=BaseColor, RGB), Specular(A: 우리는 Specular 없음)
    FTextureRHIRef GBufferC{}; // Metallic(R), Roughness(G), Ambient Occlusion(B)
    FTextureRHIRef GBufferD{}; // World Position(RGB)
//    FRDGTextureRef GBufferE{};
//    FRDGTextureRef GBufferF{};
//
//    // 모바일에서 사용하는 추가 깊이 버퍼 텍스처입니다.
//    FRDGTextureMSAA DepthAux{};
//
//    // 동적 모션 벡터를 포함하는 텍스처입니다. 기본 패스 또는 자체 속도 패스에 바인딩될 수 있습니다.
//    FRDGTextureRef Velocity{};
//
//    // (모바일 로컬 라이트 프리패스) 로컬 라이트 방향 및 색상을 포함하는 텍스처입니다.
//    FRDGTextureRef MobileLocalLightTextureA{};
//    FRDGTextureRef MobileLocalLightTextureB{};
//
//    // 화면 공간 앰비언트 오클루전 결과를 포함하는 텍스처입니다.
//    FRDGTextureRef ScreenSpaceAO{};
//
//    // 활성화된 경우 쿼드 오버드로우 디버그 보기 모드에서 사용되는 텍스처입니다.
//    FRDGTextureRef QuadOverdraw{};
//
//    // (모바일) 다음 프레임에서 모바일 PPR에 사용되는 텍스처입니다.
//    FRDGTextureRef PixelProjectedReflection{};
//
//    // 에디터 프리미티브를 합성하는 데 사용되는 텍스처들입니다. 와이어프레임 모드일 때 기본 패스에서 사용됩니다.
//#if WITH_EDITOR
//    FRDGTextureRef EditorPrimitiveColor{};
//    FRDGTextureRef EditorPrimitiveDepth{};
//#endif
};
