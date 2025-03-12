#pragma once
#include "Components/LightComponent.h"
#include "DirectionalLightComponent.generated.h"

class FLightSceneProxy;

/**
 * 평행 광선을 가진 빛 컴포넌트 입니다. 영향을 받는 모든 표면(예: 태양)에 균일한 조명을 제공합니다. 
 * 이는 정의된 광량 중요 볼륨 내의 모든 객체에 영향을 미칩니다.
 */
UCLASS()
class ENGINE_API UDirectionalLightComponent : public ULightComponent
{
	GENERATED_BODY()

public:
	virtual FVector4D GetLightPosition() const override;
	virtual FLightSceneProxy* CreateSceneProxy() override;
};