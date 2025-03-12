#pragma once
#include "Components/ActorComponent.h"
#include "Engine/EngineTypes.h"
#include "SceneComponent.generated.h"

/** SceneComponents를 이동할 때 두 FRotator가 동일한지 확인하기 위한 비교 허용 오차 */
#define SCENECOMPONENT_ROTATOR_TOLERANCE    (1.e-4f)

/** 변환을 위한 공간 */
//UENUM()
enum ERelativeTransformSpace : int
{
	/** 월드 공간 변환. */
	RTS_World,
	/** 액터 공간 변환. */
	RTS_Actor,
	/** 컴포넌트 공간 변환. */
	RTS_Component,
	/** 부모 본 공간 변환 */
	RTS_ParentBoneSpace,
};

/**
 * SceneComponent는 Transform을 가지며 Attach를 지원하지만, 렌더링이나 충돌 기능은 없습니다.
 * Useful as a 'dummy' component in the hierarchy to offset others.
 * @see [Scene Components](https://docs.unrealengine.com/latest/INT/Programming/UnrealArchitecture/Actors/Components/index.html#scenecomponents)
 */
UCLASS()
class ENGINE_API USceneComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	USceneComponent();

	/** Get the SceneComponent we are attached to. */
	//UFUNCTION(BlueprintCallable, Category = "Transformation")
	USceneComponent* GetAttachParent() const;

	/** 현재 컴포넌트의 컴포넌트-월드 변환을 가져옵니다. */
	FORCEINLINE const FTransform& GetComponentTransform() const
	{
		return ComponentToWorld;
	}

	/** 컴포넌트의 회전을 월드 공간에서 반환합니다. */
	FORCEINLINE FRotator GetComponentRotation() const
	{
		return WorldRotationCache.NormalizedQuatToRotator(GetComponentTransform().GetQuaternion());
	}

	/** 컴포넌트의 위치를 월드 공간에서 반환합니다. */
	FORCEINLINE FVector GetComponentLocation() const
	{
		return GetComponentTransform().GetLocation();
	}

	/** 컴포넌트의 회전 쿼터니언을 월드 공간에서 반환합니다. */
	FORCEINLINE FQuat GetComponentQuat() const
	{
		return GetComponentTransform().GetQuaternion();
	}

	/** 컴포넌트의 크기를 월드 공간에서 반환합니다. */
	FORCEINLINE FVector GetComponentScale() const
	{
		return GetComponentTransform().GetScale3D();
	}

	/**
	 * RelativeRotation의 실제 값을 가져옵니다.
	 * 참고로, 이것이 루트 컴포넌트(어떤 것에도 부착되지 않은)일 경우나 GetAbsoluteRotation이 true를 반환할 때는 절대 회전일 수 있습니다.
	 *
	 * 서브클래스가 RelativeRotation 속성에 직접 접근할 필요가 없도록 하기 위해 존재합니다. 이로 인해 나중에 private으로 만들 수 있습니다.
	 */
	FRotator GetRelativeRotation() const
	{
		return RelativeRotation;
	}

	/**
	 * RelativeLocation의 실제 값을 가져옵니다.
	 * 참고로, 이것이 루트 컴포넌트(어떤 것에도 부착되지 않은)일 경우나 IsUsingAbsoluteLocation이 true를 반환할 때는 절대 위치일 수 있습니다.
	 *
	 * 서브클래스가 RelativeLocation 속성에 직접 접근할 필요가 없도록 하기 위해 존재합니다. 이로 인해 나중에 private으로 만들 수 있습니다.
	 */
	FVector GetRelativeLocation() const
	{
		return RelativeLocation;
	}

	/**
	 * RelativeScale3D의 실제 값을 가져옵니다.
	 * 참고로, 이것이 루트 컴포넌트(어떤 것에도 부착되지 않은)일 경우나 GetAbsoluteScale3D가 true를 반환할 때는 절대 스케일일 수 있습니다.
	 *
	 * 서브클래스가 RelativeScale3D 속성에 직접 접근할 필요가 없도록 하기 위해 존재합니다. 이로 인해 나중에 private으로 만들 수 있습니다.
	 */
	FVector GetRelativeScale3D() const
	{
		return RelativeScale3D;
	}
	/**
	 * 월드 공간 소켓 변환을 가져옵니다.
	 * @param InSocketName 변환을 가져올 소켓 또는 본의 이름
	 * @return 소켓이 발견되면 월드 공간의 소켓 변환을 반환합니다. 그렇지 않으면 컴포넌트의 월드 공간 변환을 반환합니다.
	 */
	//UFUNCTION(BlueprintCallable, Category = "Transformation", meta = (Keywords = "Bone"))
	virtual FTransform GetSocketTransform(FName InSocketName, ERelativeTransformSpace TransformSpace = RTS_World) const;

	/**
	 * 컴포넌트의 위치를 부모와 상대적으로 설정합니다.
	 * @param NewLocation       컴포넌트의 새로운 위치를 부모와 상대적으로 설정합니다.
	 * @param SweepHitResult    스위프가 true일 경우 발생하는 충돌의 결과입니다.
	 * @param bSweep            목적지로 스위프할지 여부를 나타냅니다. 스위프 중 충돌이 발생하면 목표 지점에 도달하지 못합니다.
	 *                          루트 컴포넌트만 스위프되며, 차일드 컴포넌트는 스위프 없이 이동합니다. 충돌이 꺼져 있으면 이 옵션은 효과가 없습니다.
	 * @param bTeleport         물리 상태를 텔레포트할지 여부를 나타냅니다(이 객체에 물리적 충돌이 활성화된 경우).
	 *                          true이면 이 객체의 물리 속도는 변경되지 않습니다(따라서 래그돌 부품은 위치 변경에 영향을 받지 않습니다).
	 *                          false이면 물리 속도는 위치 변경에 따라 업데이트됩니다(래그돌 부품에 영향을 줍니다).
	 *                          CCD가 켜져 있고 텔레포트가 아닌 경우, 이는 전체 스위프 볼륨에 걸쳐 객체에 영향을 줍니다.
	 */
	void SetRelativeLocation(FVector NewLocation/*, bool bSweep = false, FHitResult* OutSweepHitResult = nullptr, ETeleportType Teleport = ETeleportType::None*/);

	/**
	 * 컴포넌트의 회전을 부모와 상대적으로 설정합니다.
	 * @param NewRotation       컴포넌트의 새로운 회전을 부모와 상대적으로 설정합니다.
	 * @param SweepHitResult    스위프가 true일 경우 발생하는 충돌의 결과입니다.
	 * @param bSweep            목적지로 스위프할지 여부를 나타냅니다 (현재 회전에 대해서는 지원되지 않습니다).
	 * @param bTeleport         물리 상태를 텔레포트할지 여부를 나타냅니다 (이 객체에 물리적 충돌이 활성화된 경우).
	 *                          true이면 이 객체의 물리 속도는 변경되지 않습니다 (따라서 래그돌 부품은 위치 변경에 영향을 받지 않습니다).
	 *                          false이면 물리 속도는 위치 변경에 따라 업데이트됩니다 (래그돌 부품에 영향을 줍니다).
	 */
	void SetRelativeRotation(FRotator NewRotation/*, bool bSweep = false, FHitResult* OutSweepHitResult = nullptr, ETeleportType Teleport = ETeleportType::None*/);
	void SetRelativeRotation(const FQuat& NewRotation/*, bool bSweep = false, FHitResult* OutSweepHitResult = nullptr, ETeleportType Teleport = ETeleportType::None*/);

	/**
	 * 컴포넌트의 변환을 부모와 상대적으로 설정합니다.
	 * @param NewTransform      컴포넌트의 새로운 변환을 부모와 상대적으로 설정합니다.
	 * @param SweepHitResult    스위프가 true일 경우 발생하는 충돌의 결과입니다.
	 * @param bSweep            목적지로 스위프할지 여부를 나타냅니다 (현재 회전에 대해서는 지원되지 않습니다).
	 * @param bTeleport         물리 상태를 텔레포트할지 여부를 나타냅니다 (이 객체에 물리적 충돌이 활성화된 경우).
	 *                          true이면 이 객체의 물리 속도는 변경되지 않습니다 (따라서 래그돌 부품은 위치 변경에 영향을 받지 않습니다).
	 *                          false이면 물리 속도는 위치 변경에 따라 업데이트됩니다 (래그돌 부품에 영향을 줍니다).
	 */
	void SetRelativeTransform(const FTransform& NewTransform/*, bool bSweep = false, FHitResult* OutSweepHitResult = nullptr, ETeleportType Teleport = ETeleportType::None*/);

	/**
	 * 컴포넌트의 위치와 회전을 부모와 상대적으로 설정합니다.
	 * @param NewLocation       컴포넌트의 새로운 위치를 부모와 상대적으로 설정합니다.
	 * @param NewRotation       컴포넌트의 새로운 회전을 부모와 상대적으로 설정합니다.
	 * @param SweepHitResult    스위프가 true일 경우 발생하는 충돌의 결과입니다.
	 * @param bSweep            목적지로 스위프할지 여부를 나타냅니다. 스위프 중 충돌이 발생하면 목표 지점에 도달하지 못합니다.
	 *                          루트 컴포넌트만 스위프되며, 차일드 컴포넌트는 스위프 없이 이동합니다. 충돌이 꺼져 있으면 이 옵션은 효과가 없습니다.
	 * @param bTeleport         물리 상태를 텔레포트할지 여부를 나타냅니다(이 객체에 물리적 충돌이 활성화된 경우).
	 *                          true이면 이 객체의 물리 속도는 변경되지 않습니다(따라서 래그돌 부품은 위치 변경에 영향을 받지 않습니다).
	 *                          false이면 물리 속도는 위치 변경에 따라 업데이트됩니다(래그돌 부품에 영향을 줍니다).
	 *                          CCD가 켜져 있고 텔레포트가 아닌 경우, 이는 전체 스위프 볼륨에 걸쳐 객체에 영향을 줍니다.
	 */
	void SetRelativeLocationAndRotation(FVector NewLocation, FRotator NewRotation/*, bool bSweep = false, FHitResult* OutSweepHitResult = nullptr, ETeleportType Teleport = ETeleportType::None*/);
	void SetRelativeLocationAndRotation(FVector NewLocation, const FQuat& NewRotation/*, bool bSweep = false, FHitResult* OutSweepHitResult = nullptr, ETeleportType Teleport = ETeleportType::None*/);

	/** 컴포넌트의 비균일 스케일을 부모와 상대적으로 설정합니다. */
	void SetRelativeScale3D(FVector NewScale3D);

	void SetWorldTransform(const FTransform& NewTransform/*, bool bSweep = false, FHitResult* OutSweepHitResult = nullptr, ETeleportType Teleport = ETeleportType::None*/);
	void SetWorldLocation(FVector NewLocation/*, bool bSweep = false, FHitResult* OutSweepHitResult = nullptr, ETeleportType Teleport = ETeleportType::None*/);

public:
	virtual void UpdateComponentToWorld()
	{
		UpdateComponentToWorldWithParent(GetAttachParent(), /*GetAttachSocketName(), UpdateTransformFlags,*/ RelativeRotationCache.RotatorToQuat(GetRelativeRotation())/*, Teleport*/);
	}

protected:
	virtual bool ShouldCreateRenderState() const override { return true; }

protected:
	/** 이 컴포넌트의 새로운 ComponentToWorld 변환 값을 계산합니다.
		Parent는 선택 사항이며, 임의의 USceneComponent를 기반으로 ComponentToWorld를 계산하는 데 사용할 수 있습니다.
		Parent가 전달되지 않으면 컴포넌트의 AttachParent를 사용합니다. */
	FORCEINLINE FTransform CalcNewComponentToWorld(const FTransform& NewRelativeTransform, const USceneComponent* Parent = nullptr, FName SocketName = NAME_None) const
	{
		// 아직 지원하지 않음
		// SocketName = Parent ? SocketName : GetAttachSocketName();
		SocketName = NAME_None; // 강제로 리셋

		Parent = Parent ? Parent : GetAttachParent();
		if (Parent)
		{
			//const bool bGeneral = IsUsingAbsoluteLocation() || IsUsingAbsoluteRotation() || IsUsingAbsoluteScale();
			//if (!bGeneral)
			{
				return NewRelativeTransform * Parent->GetSocketTransform(SocketName);
			}

			//return CalcNewComponentToWorld_GeneralCase(NewRelativeTransform, Parent, SocketName);
		}
		else
		{
			return NewRelativeTransform;
		}
	}

public:
	/**
	 * 컴포넌트가 등록될 때 연결될 Attach Parent와 SocketName을 초기화합니다.
	 * 일반적으로 Owning Actor의 생성자에서 호출되며, 컴포넌트가 등록되지 않은 상태에서 AttachToComponent보다 우선하여 사용해야 합니다.
	 * @param  InParent             연결될 부모 객체.
	 * @param  InSocketName         부모 객체의 소켓에 연결하기 위한 선택적 소켓 이름.
	 */
	void SetupAttachment(USceneComponent* InParent/*, FName InSocketName = NAME_None*/);

	/**
	 * 이 컴포넌트가 제공된 컴포넌트에 연결되어 있는지 확인하기 위해 첨부 체인을 따라 올라갑니다.
	 * 만약 TestComp가 이 컴포넌트와 동일한 경우, false를 반환합니다.
	*/
	bool IsAttachedTo(const USceneComponent* TestComp) const;

private:
	/**
	 * 이 인스턴스에 다른 부작용을 일으키지 않고 AttachParent의 값을 설정합니다.
	 * 다른 시스템은 이 값을 변경할 때 알림을 받을 수 있습니다.
	 */
	void SetAttachParent(USceneComponent* NewAttachParent);

	void UpdateComponentToWorldWithParent(USceneComponent* Parent, /*FName SocketName, */const FQuat& RelativeRotationQuat/*, ETeleportType Teleport = ETeleportType::None*/);

private:
	/** 부모에 대한 컴포넌트의 위치 */
	UPROPERTY(EditAnywhere)
	FVector RelativeLocation = FVector::Zero;

	/** 부모에 대한 컴포넌트의 회전 */
	UPROPERTY(EditAnywhere)
	FRotator RelativeRotation = FRotator::ZeroRotator;

	/**
	*   부모에 대한 컴포넌트의 비균일 스케일링
	*   스케일링은 항상 로컬 공간에서 적용됨(전단 변형 등 없음)
	*/
	UPROPERTY(EditAnywhere)
	FVector RelativeScale3D = FVector::One;

private:
	/** RelativeLocation/Rotation/Scale을 기반으로 ComponentToWorld를 업데이트한 적이 있는 경우 true. 초기화 시 이를 확인하기 위해 사용됩니다. */
	uint8 bComponentToWorldUpdated : 1 = false;

private:
	/** 현재 연결된 객체. 유효한 경우, RelativeLocation 등은 이 객체를 기준으로 사용됩니다 */
	//UPROPERTY()
	TEnginePtr<USceneComponent> AttachParent;

	/**
	 * 자식 SceneComponents의 목록입니다.
	 */
	//UPROPERTY()
	TArray<TEnginePtr<USceneComponent>> AttachChildren;

private:
	/** 가능하면 Quat<->Rotator 변환을 피하는 캐시입니다. GetComponentTransform().GetRotation()과 함께 사용해야 합니다. */
	FRotationConversionCache WorldRotationCache;

	/** 가능한 경우 Quat<->Rotator 변환을 피하는 캐시. RelativeRotation과 함께 사용해야 합니다. */
	FRotationConversionCache RelativeRotationCache;

	/** 현재 컴포넌트의 Transform, 월드 기준 */
	FTransform ComponentToWorld;
};