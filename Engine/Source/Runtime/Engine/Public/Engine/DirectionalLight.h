#pragma once
#include "Light.h"
#include "DirectionalLight.generated.h"

UCLASS()
class ENGINE_API ADirectionalLight : public ALight
{
	GENERATED_BODY()

public:
	ADirectionalLight(const FObjectInitializer& ObjectInitializer);
};