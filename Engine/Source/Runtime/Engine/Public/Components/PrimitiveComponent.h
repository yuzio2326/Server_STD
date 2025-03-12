#pragma once
#include "Components/SceneComponent.h"
#include "PrimitiveComponent.generated.h"

class FPrimitiveSceneProxy;

/**
 * PrimitiveComponent는 렌더링되거나 충돌 데이터로 사용하기 위해 일정한 형태의 지오메트리를 포함하거나 생성하는 SceneComponent입니다.
 * 여러 유형의 지오메트리를 위한 여러 서브클래스가 있지만, 가장 일반적인 것은 ShapeComponent(캡슐, 구, 박스), StaticMeshComponent 및 SkeletalMeshComponent입니다.
 * ShapeComponent는 충돌 감지를 위해 사용되는 지오메트리를 생성하지만 렌더링되지 않으며, StaticMeshComponent와 SkeletalMeshComponent는 렌더링되는 사전 제작된 지오메트리를 포함하지만 충돌 감지에도 사용할 수 있습니다.
 */
UCLASS()
class ENGINE_API UPrimitiveComponent : public USceneComponent
{
	friend class FScene;
	GENERATED_BODY()
public:
	UPrimitiveComponent();
	~UPrimitiveComponent();

	FMatrix GetRenderMatrix() const;

public:
	/** 이전 프리미티브의 Scene Info 포인터, 이제 SceneData에 있는 포인터로 대체되었지만, 여전히 사용되기 때문에 여기에 남아 있습니다. */
	FPrimitiveSceneProxy* SceneProxy = nullptr;

	/**
	 * 이 컴포넌트의 렌더링 스레드 정보를 생성하는 데 사용됩니다.
	 * @warning 여러 스레드에서 동시에 호출됩니다(그러나 동일한 컴포넌트는 동시에 호출되지 않습니다).
	 */
	virtual void CreateRenderState_Concurrent(FRegisterComponentContext* Context);

private:
	virtual FPrimitiveSceneProxy* CreateSceneProxy() { return nullptr; }
};