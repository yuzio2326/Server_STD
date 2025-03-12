#pragma once
#include "CoreMinimal.h"

class UPrimitiveComponent;

/**
 * UPrimitiveComponent를 렌더링하기 위해 게임 스레드와 병렬로 미러링되는 데이터를 캡슐화합니다.
 * 이는 다양한 프리미티브 타입을 지원하기 위해 서브클래싱(subclassing)될 의도로 설계되었습니다.
 */
class FPrimitiveSceneProxy
{
public:
	/** Initialization constructor. */
	ENGINE_API FPrimitiveSceneProxy(UPrimitiveComponent* InComponent, FName ResourceName = NAME_None);

	/** Copy constructor. */
	FPrimitiveSceneProxy(FPrimitiveSceneProxy const&) = default;

	/** Virtual destructor. */
	ENGINE_API virtual ~FPrimitiveSceneProxy();

	UPrimitiveComponent* GetPrimitiveComponent() { return Component; }

	void SetTransform(const FMatrix& InLocalToWorld) { LocalToWorld = InLocalToWorld; }
	const FMatrix& GetTransform() const { return LocalToWorld; }

protected:
	UPrimitiveComponent* Component = nullptr;

	/** 프리미티브의 로컬에서 월드로의 변환 행렬. */
	FMatrix LocalToWorld;

	/** 컴포넌트에서 사용하는 리소스의 이름. */
	FName ResourceName;
};