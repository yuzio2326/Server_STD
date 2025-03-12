#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "StaticMeshActor.generated.h"

/**
 * StaticMeshActor는 월드 내의 UStaticMesh 인스턴스입니다.
 * Static meshes는 애니메이션되거나 변형되지 않는 기하학적 도형으로, 다른 유형의 기하학적 도형보다 렌더링 효율이 높습니다.
 * Content Browser에서 레벨로 드래그된 Static meshes는 자동으로 StaticMeshActor로 변환됩니다.
 *
 * @see https://docs.unrealengine.com/latest/INT/Engine/Actors/StaticMeshActor/
 * @see UStaticMesh
 */
UCLASS()
class AStaticMeshActor : public AActor
{
	GENERATED_BODY()
public:
	AStaticMeshActor();

	virtual void OnPropertyChanged(FProperty&) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	//UPROPERTY()
	UStaticMeshComponent* StaticMeshComponent;
	UStaticMeshComponent* ChildStaticMeshComponent;
};