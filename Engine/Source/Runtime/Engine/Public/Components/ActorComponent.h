#pragma once
#include "CoreMinimal.h"
#include "ActorComponent.generated.h"

class AActor;
class UWorld;
class UPrimitiveComponent;

class FRegisterComponentContext
{
public:
	FRegisterComponentContext(UWorld* InWorld)
		: World(InWorld)
	{
	}

	void AddPrimitive(UPrimitiveComponent* PrimitiveComponent)
	{
		//_ASSERT(!AddPrimitiveBatches.contain(PrimitiveComponent));
		AddPrimitiveBatches.emplace_back(PrimitiveComponent);
	}

	//void AddSendRenderDynamicData(UPrimitiveComponent* PrimitiveComponent)
	//{
	//	//checkSlow(!SendRenderDynamicDataPrimitives.Contains(PrimitiveComponent));
	//	SendRenderDynamicDataPrimitives.Add(PrimitiveComponent);
	//}

	ENGINE_API static void SendRenderDynamicData(FRegisterComponentContext* Context, UPrimitiveComponent* PrimitiveComponent);

	int32 Count() const { return (int32)AddPrimitiveBatches.size(); }
	void Process();

	UWorld* GetWorld() const { return World; }

private:
	UWorld* World;
	TArray<UPrimitiveComponent*> AddPrimitiveBatches;
	//TArray<UPrimitiveComponent*> SendRenderDynamicDataPrimitives;
};


/**
 * ActorComponent는 다양한 유형의 액터에 추가될 수 있는 재사용 가능한 동작을 정의하는 컴포넌트의 기본 클래스입니다.
 * Transform을 가진 ActorComponent는 SceneComponent로 알려져 있으며, 렌더링할 수 있는 ActorComponent는 PrimitiveComponent입니다.
 *
 * @see [ActorComponent](https://docs.unrealengine.com/latest/INT/Programming/UnrealArchitecture/Actors/Components/index.html#actorcomponents)
 * @see USceneComponent
 * @see UPrimitiveComponent
 */
UCLASS()
class ENGINE_API UActorComponent : public UObject
{
	GENERATED_BODY()
public:
	UActorComponent();

public:
	/** 이 컴포넌트에 대한 렌더 '상태' (예: Scene Proxy)가 생성되었는지 여부를 반환합니다. */
	bool IsRenderStateCreated() const
	{
		return bRenderStateCreated;
	}

	virtual void PostInitProperties();

public:
	/** Outer 체인을 따라 이 컴포넌트를 '소유'하는 AActor를 가져옵니다. */
	//UFUNCTION(BlueprintCallable, Category = "Components", meta = (Keywords = "Actor Owning Parent"))
	AActor* GetOwner() const;

	/** 이 컴포넌트가 현재 등록되어 있는지 확인합니다. */
	FORCEINLINE bool IsRegistered() const { return bRegistered; }
	
	/** 이 컴포넌트를 등록하여 렌더링/물리 상태를 생성합니다. 또한, 이미 존재하지 않는 경우 Outer 액터의 컴포넌트 배열에 자신을 추가합니다. */
	void RegisterComponent();

	/**
	 * 컴포넌트를 특정 월드에 등록하여 시각적/물리적 상태를 생성합니다.
	 * @param InWorld - 컴포넌트를 등록할 월드입니다.
	 */
	void RegisterComponentWithWorld(UWorld* InWorld, FRegisterComponentContext* Context = nullptr);

	/** 컴포넌트가 생성될 때 호출됩니다(로드되는 것이 아님). 이는 에디터에서나 게임 플레이 중에 발생할 수 있습니다. */
	virtual void OnComponentCreated();

	/** OnRegister, CreateRenderState_Concurrent 및 OnCreatePhysicsState를 호출합니다. */
	void ExecuteRegisterEvents(FRegisterComponentContext* Context = nullptr);

protected:
	/**
	 * 컴포넌트가 등록된 후, Scene이 설정된 후에 호출되지만 CreateRenderState_Concurrent 또는 OnCreatePhysicsState가 호출되기 전입니다.
	 */
	virtual void OnRegister();

	/** 컴포넌트에서 월드로 변환하는 값 재계산 */
	virtual void UpdateComponentToWorld(/*EUpdateTransformFlags UpdateTransformFlags = EUpdateTransformFlags::None, ETeleportType Teleport = ETeleportType::None*/) {}

	/** CreateRenderState()가 호출되어야 하는 경우 true를 반환합니다. */
	virtual bool ShouldCreateRenderState() const
	{
		return false;
	}

protected:
	/**
	 * 이 컴포넌트의 렌더링 스레드 정보를 생성하는 데 사용됩니다.
	 * @warning 여러 스레드에서 동시에 호출됩니다(그러나 동일한 컴포넌트는 동시에 호출되지 않습니다).
	 */
	virtual void CreateRenderState_Concurrent(FRegisterComponentContext* Context);

public:
	/**
	 * SceneComponent를 활성화하며, 네이티브 자식 클래스에서 재정의해야 합니다.
	 * @param bReset - ShouldActivate가 false를 반환하더라도 활성화를 해야 하는지 여부입니다.
	 */
	//UFUNCTION(BlueprintCallable, Category = "Components|Activation", meta = (UnsafeDuringActorConstruction = "true"))
	virtual void Activate(bool bReset = false);
	
	/**
	 * 이 컴포넌트의 tick 기능을 활성화하거나 비활성화합니다. 함수가 등록된 경우에만 효과가 있습니다.
	 *
	 * @param bEnabled - 활성화할지 여부입니다.
	 */
	//UFUNCTION(BlueprintCallable, Category = "Components|Tick")
	virtual void SetComponentTickEnabled(bool bEnabled);

	/** InitializeComponent가 호출되었지만 UninitializeComponent가 아직 호출되지 않음을 나타냅니다. */
	bool HasBeenInitialized() const { return bHasBeenInitialized; }

	/**
	 * 컴포넌트를 초기화합니다. 레벨 시작 또는 액터 스폰 시 발생합니다. 이는 BeginPlay(Actor 또는 Component)보다 이전입니다.
	 * 레벨에 있는 모든 컴포넌트는 로드 시 초기화되며, 어떤 액터/컴포넌트도 BeginPlay를 받기 전에 초기화됩니다.
	 * 컴포넌트가 등록되고 bWantsInitializeComponent이 true여야 합니다.
	 */
	virtual void InitializeComponent();

	/** BeginPlay가 호출되었지만 EndPlay이 아직 호출되지 않음을 나타냅니다. */
	bool HasBegunPlay() const { return bHasBegunPlay; }

	/**
	 * 컴포넌트의 플레이를 시작합니다.
	 * 소유 액터가 플레이를 시작하거나, 액터가 이미 플레이를 시작했을 때 컴포넌트가 생성될 때 호출됩니다.
	 * Actor BeginPlay는 보통 PostInitializeComponents 다음에 발생하지만, 네트워크 액터나 자식 액터의 경우 지연될 수 있습니다.
	 * 컴포넌트가 등록되고 초기화되어야 합니다.
	 */
	virtual void BeginPlay();

public:
	/** 컴포넌트가 생성될 때 활성화되는지 또는 명시적으로 활성화되어야 하는지 여부입니다. */
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Activation)
	uint8 bAutoActivate : 1 = false;

protected:
	/**
	 * 이 ActorComponent가 현재 씬에 등록되어 있는지 여부를 나타냅니다.
	 */
	uint8 bRegistered : 1 = false;

	/** 이 컴포넌트에 대한 렌더 상태가 현재 생성되었는지 여부 */
	uint8 bRenderStateCreated : 1 = false;

	/** BeginPlay가 호출되었지만 EndPlay이 아직 호출되지 않음을 나타냅니다. */
	uint8 bHasBegunPlay : 1 = false;

public:
	/**
	 * 컴포넌트가 활성 상태인지 여부를 반환합니다.
	 * @return - 컴포넌트의 활성 상태입니다.
	 */
	//UFUNCTION(BlueprintCallable, Category = "Components|Activation", meta = (UnsafeDuringActorConstruction = "true"))
	bool IsActive() const { return bIsActive; }

	/**
	 * bIsActive의 값을 설정하지만 이 인스턴스에 다른 부작용을 일으키지 않습니다.
	 *
	 * 대부분의 경우에는 Activate, Deactivate, SetActive를 사용하는 것이 좋습니다. 왜냐하면 이들은 가상 동작을 존중하기 때문입니다.
	 */
	void SetActiveFlag(const bool bNewIsActive);

protected:
	/** 이 컴포넌트가 정상적으로 활성화될 수 있는 상태인지 여부를 반환합니다. */
	virtual bool ShouldActivate() const;

private:
	/** 이 컴포넌트가 현재 활성 상태인지 여부 */
	//UPROPERTY(transient, ReplicatedUsing = OnRep_IsActive)
	uint8 bIsActive : 1 = false;

	/** InitializeComponent가 호출되었지만 UninitializeComponent가 아직 호출되지 않음을 나타냅니다. */
	uint8 bHasBeenInitialized : 1 = false;

private:
	AActor* GetActorOwnerNoninline() const;

private:
	/** 소유 액터에 대한 캐시된 포인터 */
	mutable AActor* OwnerPrivate = nullptr;

	/**
	 * 이 컴포넌트가 현재 등록된 월드에 대한 포인터입니다.
	 * 컴포넌트가 등록된 경우에만 NULL이 아닙니다.
	 */
	UWorld* WorldPrivate = nullptr;

private:
	/** OnCreatedComponent가 호출되었지만 OnDestroyedComponent는 아직 호출되지 않았음을 나타냅니다. */
	uint8 bHasBeenCreated : 1 = false;
};