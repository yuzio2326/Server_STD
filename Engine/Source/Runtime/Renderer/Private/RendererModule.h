#pragma once

#include "CoreMinimal.h"
#include "RendererInterface.h"
#include "Shader.h"

class FSceneInterface;

/** 렌더러 모듈 구현입니다. */
class FRendererModule final : public IRendererModule
{
public:
	FRendererModule();

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

public:
	virtual FSceneInterface* AllocateScene(UWorld* World, ERHIFeatureLevel::Type InFeatureLevel) override;
	virtual void RemoveScene(FSceneInterface* Scene) override;

	virtual void BeginRenderingViewFamily(FSceneViewFamily* ViewFamily) override;

private:
	TSet<FSceneInterface*> AllocatedScenes;
};