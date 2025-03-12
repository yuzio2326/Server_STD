#pragma once
#include "PrimitiveSceneProxy.h"

class UStaticMeshComponent;
struct FStaticMeshRenderData;

/**
 * 정적 메쉬 컴포넌트의 장면 프록시.
 */
class FStaticMeshSceneProxy : public FPrimitiveSceneProxy
{
public:
	/** Initialization constructor. */
	ENGINE_API FStaticMeshSceneProxy(UStaticMeshComponent* Component/*, bool bForceLODsShareStaticLighting*/);

protected:
	TArray<FStaticMeshRenderData>* RenderData = nullptr;
};