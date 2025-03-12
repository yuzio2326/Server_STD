#include "PrimitiveSceneProxy.h"
#include "Components/PrimitiveComponent.h"

FPrimitiveSceneProxy::FPrimitiveSceneProxy(UPrimitiveComponent* InComponent, FName ResourceName)
	: Component(InComponent), ResourceName(ResourceName)
{
}

FPrimitiveSceneProxy::~FPrimitiveSceneProxy()
{
}
