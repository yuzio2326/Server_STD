#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "SceneInterface.h"

UPrimitiveComponent::UPrimitiveComponent()
{
}

UPrimitiveComponent::~UPrimitiveComponent()
{
	if (bRegistered)
	{
		if (GetWorld() && GetWorld()->Scene)
		{
			GetWorld()->Scene->RemovePrimitive(this);
		}
	}
}

FMatrix UPrimitiveComponent::GetRenderMatrix() const
{
	return GetComponentTransform().GetMatrix();
}

void UPrimitiveComponent::CreateRenderState_Concurrent(FRegisterComponentContext* Context)
{
	Super::CreateRenderState_Concurrent(Context);

	if (Context != nullptr)
	{
		Context->AddPrimitive(this);
	}
	else
	{
		GetWorld()->Scene->AddPrimitive(this);
	}
}
