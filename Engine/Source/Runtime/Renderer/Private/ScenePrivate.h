#pragma once
#include "CoreMinimal.h"
#include "SceneInterface.h"

class FPrimitiveSceneProxy;
class FLightSceneProxy;

/**
 * 렌더러 모듈에 비공개된 렌더러 씬입니다.
 * 일반적으로 이는 UWorld의 렌더러 버전이지만, UWorld가 없는 에디터에서 미리보기를 위해 FScene이 생성될 수 있습니다.
 * 씬은 뷰 또는 프레임과 독립적인 렌더러 상태를 저장하며, 주요 작업은 기본 프리미티브와 조명을 추가하고 제거하는 것입니다.
 */
class FScene : public FSceneInterface
{
public:

    /** 씬과 연결된 선택적 월드입니다. */
    UWorld* World;

    /** Initialization constructor. */
    FScene(UWorld* InWorld, /*bool bInRequiresHitProxies, bool bInIsEditorScene, bool bCreateFXSystem,*/ ERHIFeatureLevel::Type InFeatureLevel);
    virtual ~FScene();

    // FSceneInterface interface.
    virtual void AddPrimitive(UPrimitiveComponent* Primitive) override;
    virtual void RemovePrimitive(UPrimitiveComponent* Primitive) override;
    virtual void AddLight(ULightComponent* Light) override;
    virtual void RemoveLight(ULightComponent* Light) override;
    virtual void Release() override;

public:
    TArray<FPrimitiveSceneProxy*> PrimitiveSceneProxies;
    TArray<FLightSceneProxy*> LightSceneProxies;
};