#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "StaticMeshSceneProxy.h"

UStaticMeshComponent::UStaticMeshComponent()
{
}

void UStaticMeshComponent::SetStaticMesh(TEnginePtr<UStaticMesh> NewStaticMesh)
{
	if (StaticMesh == NewStaticMesh)
	{
		return;
	}

	StaticMesh = NewStaticMesh;
	OverrideMaterials.clear();
	OverrideMaterials.resize(StaticMesh->GetRenderDataCount());
}

TEnginePtr<UMaterial> UStaticMeshComponent::GetMaterial(const int32 MaterialIndex)
{
	if (OverrideMaterials[MaterialIndex] == nullptr)
	{
		return StaticMesh->GetMaterial(MaterialIndex);
	}
	else
	{
		return OverrideMaterials[MaterialIndex];
	}
}

void UStaticMeshComponent::SetMaterial(TEnginePtr<UMaterial> NewMaterial, int32 MaterialIndex)
{
	OverrideMaterials[MaterialIndex] = NewMaterial;	
}

uint32 UStaticMeshComponent::GetMaterialCount()
{
	return StaticMesh->GetRenderDataCount();
}

bool UStaticMeshComponent::ShouldCreateRenderState() const
{
	if (!Super::ShouldCreateRenderState())
	{
		E_LOG(Log, TEXT("ShouldCreateRenderState returned false for {} (Base class was false)"), GetName());
		return false;
	}

	// 유효하지 않거나 컴파일 중인 정적 메쉬에 대한 렌더 상태를 생성하는 것을 특히 피하는 것이 중요합니다.
	// 셰이더 컴파일러가 렌더 상태는 있지만 렌더 프록시가 없는 컴포넌트에서 재료를 교체하려고 할 수 있습니다.
	// 이 경우, 실행할 필요가 없는 렌더 상태 재생성 코드에서 게임 스레드 지연이 크게 발생할 수 있습니다.
	if (GetStaticMesh() == nullptr)
	{
		E_LOG(Log, TEXT("ShouldCreateRenderState returned false for {} (StaticMesh is null)"), GetName());
		return false;
	}

	return true;
}

void UStaticMeshComponent::OnPropertyChanged(FProperty& InProperty)
{
	Super::OnPropertyChanged(InProperty);

	if (InProperty.Name == "RasterizerState")
	{
		for (auto& It : OverrideMaterials)
		{
			It->SetRasterizerState((ERasterizerState)RasterizerState);
		}
	}
	else if (InProperty.Name == "OverrideMaterial1")
	{
		SetMaterial(OverrideMaterial1, 0);
	}
}

FPrimitiveSceneProxy* UStaticMeshComponent::CreateSceneProxy()
{
	if (GetStaticMesh() == nullptr)
	{
		E_LOG(Log, TEXT("Skipping CreateSceneProxy for StaticMeshComponent {} (StaticMesh is null)"), GetName());
		return nullptr;
	}

	if (GetStaticMesh()->GetRenderData().empty())
	{
		E_LOG(Log, TEXT("Skipping CreateSceneProxy for StaticMeshComponent {} (RenderData is null)"), GetName());
		return nullptr;
	}

	FStaticMeshSceneProxy* Proxy = new FStaticMeshSceneProxy(this);
	
	SceneProxy = Proxy;

	return Proxy;
}
