#pragma once
#include "Components/LightComponentBase.h"
#include "LightComponent.generated.h"

UCLASS()
class ENGINE_API ULightComponent : public ULightComponentBase
{
	GENERATED_BODY()

public:
	virtual ~ULightComponent();

	/** The light's scene info. */
	class FLightSceneProxy* SceneProxy;

	virtual class FLightSceneProxy* CreateSceneProxy() /*const*/
	{
		return NULL;
	}

	/* 빛의 방향을 얻어온다 */
	FVector GetDirection() const;
	virtual FVector4D GetLightPosition() const { _ASSERT(false); return FVector4D::Zero; }

protected:
	virtual void CreateRenderState_Concurrent(FRegisterComponentContext* Context) override;
};