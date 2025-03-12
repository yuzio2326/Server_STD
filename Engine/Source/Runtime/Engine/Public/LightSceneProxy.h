#pragma once

#include "GameFramework/Actor.h"

class ULightComponent;
struct FLightShaderParameters;

/**
 * 렌더링 스레드에서 조명을 렌더링하는 데 사용되는 데이터를 캡슐화합니다.
 * 생성자는 게임 스레드에서 호출되며, 그 후에는 렌더링 스레드가 객체를 소유합니다.
 * FLightSceneProxy는 엔진 모듈에 있으며 다양한 유형의 조명을 구현하기 위해 서브클래스화됩니다.
 */
class ENGINE_API FLightSceneProxy
{
public:
	/** Initialization constructor. */
	FLightSceneProxy(/*const*/ ULightComponent* InLightComponent);
	virtual ~FLightSceneProxy();

	/**
	 * 참고: 렌더링 스레드는 UObjects를 역참조해서는 안 됩니다!
	 * 게임 스레드는 UObject 상태를 소유하며 언제든지 그것들을 쓸 수 있습니다.
	 * 장면 프록시에서 데이터를 복제하고 그것을 대신 액세스하십시오.
	 */
	// 우리는 해당사항 없음
	inline ULightComponent* GetLightComponent() const { return LightComponent; }

	virtual void GetLightShaderParameters(FLightShaderParameters& OutLightParameters) {}

	/**
	 * 빛 프록시의 캐시된 변환을 업데이트합니다.
	 * @param InLightToWorld - 새로운 빛-월드 변환입니다.
	 * @param InPosition - 새로운 빛의 위치입니다.
	 */
	void SetTransform(const FMatrix& InLightToWorld, const FVector4D& InPosition);

	/** 빛의 색상을 업데이트합니다. */
	void SetColor(const FLinearColor& InColor);

protected:

	friend class FScene;

	/** 빛 구성 요소입니다. */
	/*const*/ ULightComponent* LightComponent;

	/**
	빛의 동차 위치(homogeneous position)입니다.
	빛의 동차 위치(homogeneous position)는 컴퓨터 그래픽스에서 빛의 위치를 4차원 벡터로 표현하는 것을 의미합니다.
	이는 전통적인 3차원 좌표 대신 동차 좌표를 사용함으로써 변환 행렬과 함께 더 유연하고 정확한 계산을 가능하게 합니다.
	동차 좌표는 3차원 벡터 (x, y, z)에 하나의 차원 (w)을 더하여 (x, y, z, w)로 표현합니다.
	이 w 값을 통해 변환을 보다 쉽게 적용할 수 있으며, 특히 원근 투영과 같은 변환에서 유리합니다.
	*/
	FVector4D Position;

	/** 빛의 색상입니다. */
	FLinearColor Color;

	/** 월드 공간에서 빛 공간으로의 변환입니다. */
	FMatrix WorldToLight;

	/** 빛 공간에서 월드 공간으로의 변환입니다. */
	FMatrix LightToWorld;

	inline FVector GetDirection() const { return FVector(WorldToLight.m[0][0], WorldToLight.m[1][0], WorldToLight.m[2][0]); };
};