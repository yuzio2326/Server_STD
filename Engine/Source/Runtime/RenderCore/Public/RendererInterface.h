#pragma once
#include "CoreMinimal.h"
#include "RHI.h"

class FSceneInterface;
class FSceneViewFamily;

/**
 * 렌더러 모듈의 공개 인터페이스입니다.
 */
class IRendererModule : public IModuleInterface
{
public:
	/**
	 * 주어진 월드에 대해 private FScene 구현의 새 인스턴스를 할당합니다.
	 * @param World - 씬과 연결할 선택적 월드입니다.
	 * @param bInRequiresHitProxies - 씬에서 히트 프록시를 렌더링해야 하는지 여부를 나타냅니다.
	 */
	virtual FSceneInterface* AllocateScene(UWorld* World, /*bool bInRequiresHitProxies, bool bCreateFXSystem,*/ ERHIFeatureLevel::Type InFeatureLevel) = 0;
	virtual void RemoveScene(FSceneInterface* Scene) = 0;

	virtual void BeginRenderingViewFamily(FSceneViewFamily* ViewFamily) = 0;
};