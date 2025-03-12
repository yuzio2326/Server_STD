#include "LightSceneProxy.h"

FLightSceneProxy::FLightSceneProxy(ULightComponent* InLightComponent)
	: LightComponent(InLightComponent)
{
}

FLightSceneProxy::~FLightSceneProxy() = default;

void FLightSceneProxy::SetTransform(const FMatrix& InLightToWorld, const FVector4D& InPosition)
{
	LightToWorld = InLightToWorld;
	WorldToLight = LightToWorld.Invert();
	Position = InPosition;
}

void FLightSceneProxy::SetColor(const FLinearColor& InColor)
{
	Color = InColor;
}
