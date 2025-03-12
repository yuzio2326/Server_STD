#include "StaticMeshSceneProxy.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"

FStaticMeshSceneProxy::FStaticMeshSceneProxy(UStaticMeshComponent* Component)
	: FPrimitiveSceneProxy(Component, Component->GetStaticMesh()->GetName())
{
	UStaticMesh* StaticMesh = Component->GetStaticMesh();
	RenderData = &StaticMesh->GetRenderData();
}
