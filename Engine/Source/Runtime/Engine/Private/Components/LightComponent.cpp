#include "Components/LightComponent.h"
#include "Engine/World.h"
#include "SceneInterface.h"

ULightComponent::~ULightComponent()
{
	if (bRegistered)
	{
		if (GetWorld() && GetWorld()->Scene)
		{
			GetWorld()->Scene->RemoveLight(this);
		}
	}
}

FVector ULightComponent::GetDirection() const
{
	return GetComponentTransform().GetUnitAxis(EAxis::X);
}

void ULightComponent::CreateRenderState_Concurrent(FRegisterComponentContext* Context)
{
	Super::CreateRenderState_Concurrent(Context);

	UWorld* World = GetWorld();
	World->Scene->AddLight(this);
}
