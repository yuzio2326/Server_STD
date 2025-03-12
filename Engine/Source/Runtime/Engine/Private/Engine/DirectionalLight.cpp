#include "Engine/DirectionalLight.h"
#include "Components/DirectionalLightComponent.h"

ADirectionalLight::ADirectionalLight(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UDirectionalLightComponent>(TEXT("LightComponent0")))
{
}
