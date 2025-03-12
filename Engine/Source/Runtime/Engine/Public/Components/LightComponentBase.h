#pragma once
#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "RHIDefinitions.h"
#include "LightComponentBase.generated.h"

UCLASS(/*Abstract*/)
class ENGINE_API ULightComponentBase : public USceneComponent
{
	GENERATED_BODY()

public:
	FLinearColor GetLightColor() const { return FLinearColor(LightColor); }
	void SetLightColor(const FLinearColor& InColor) { LightColor = FVector(InColor.x, InColor.y, InColor.z); }

protected:
	/** 
     * 빛의 색을 필터링합니다.
     * 이로 인해 빛의 유효 강도가 변경될 수 있습니다.
     */
	UPROPERTY(EditAnywhere)
	FVector LightColor = FVector::One;
};

/** 조명을 렌더링하기 위한 셰이더 매개변수 구조입니다. */
struct ENGINE_API MS_ALIGN(SHADER_PARAMETER_STRUCT_ALIGNMENT) FLightShaderParameters
{
	// 빛의 색상입니다.
	FLinearColor LightColor;

	// 적용되는 경우 빛의 방향입니다
	FVector3D LightDirection;
	float Padding; // 16 바이트 정렬을 위한 Padding
};