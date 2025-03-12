#pragma once
#include "Components/PrimitiveComponent.h"
#include "MeshComponent.generated.h"

/**
 * MeshComponent는 렌더링 가능한 삼각형 모음의 인스턴스인 모든 컴포넌트의 추상 기반입니다.
 *
 * @see UStaticMeshComponent
 * @see USkeletalMeshComponent
 */
UCLASS()
class ENGINE_API UMeshComponent : public UPrimitiveComponent
{
	GENERATED_BODY()
public:
	UMeshComponent();
};