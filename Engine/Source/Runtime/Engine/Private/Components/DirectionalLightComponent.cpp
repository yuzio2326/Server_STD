#include "Components/DirectionalLightComponent.h"
#include "LightSceneProxy.h"

/* LWC 최대 월드 크기, 약 87,960,930.2 km 전체, 또는 원점에서 43,980,465.1 km */
#define UE_LARGE_WORLD_MAX			8796093022208.0
#define WORLD_MAX					(UE_LARGE_WORLD_MAX)

FVector4D UDirectionalLightComponent::GetLightPosition() const
{
	FVector3D LightDirection = -GetDirection() * WORLD_MAX;
	return FVector4D(LightDirection.x, LightDirection.y, LightDirection.z, 0.f);
}

/**
 * The scene info for a directional light.
 */
class FDirectionalLightSceneProxy : public FLightSceneProxy
{
public:
	FDirectionalLightSceneProxy(UDirectionalLightComponent* Component)
		: FLightSceneProxy(Component)
	{
	}

	virtual void GetLightShaderParameters(FLightShaderParameters& OutLightParameters)
	{
		LightComponent->UpdateComponentToWorld();
		SetTransform(LightComponent->GetComponentTransform().ToMatrixNoScale(), LightComponent->GetLightPosition());

		Color = LightComponent->GetLightColor();

		OutLightParameters.LightColor = Color;
		OutLightParameters.LightDirection = -GetDirection();
	}
};

FLightSceneProxy* UDirectionalLightComponent::CreateSceneProxy()
{
	return new FDirectionalLightSceneProxy(this);
}