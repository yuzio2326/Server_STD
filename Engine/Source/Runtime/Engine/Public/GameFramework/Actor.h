#pragma once
#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Actor.generated.h"

UENUM()
enum class ESpawnActorScaleMethod : uint8
{
	/** 액터의 루트 컴포넌트의 기본 크기를 무시하고 SpawnTransform 파라미터의 값으로 고정 설정합니다 */
	OverrideRootScale, //                      UMETA(DisplayName = "Override Root Component Scale"),
	/** SpawnTransform 파라미터의 값을 액터의 루트 컴포넌트의 기본 크기와 곱합니다 */
	MultiplyWithRoot, //                       UMETA(DisplayName = "Multiply Scale With Root Component Scale"),
	SelectDefaultAtRuntime, //                 UMETA(Hidden),
};

class APawn;
class ULevel;

// Editor
// - Register

// GamePlay
// - Register
// - Initialize (Comp Active)

UCLASS()
class ENGINE_API AActor : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * 이 액터를 볼 때 카메라 뷰포인트를 계산합니다.
	 *
	 * @param   DeltaTime   마지막 업데이트 이후 델타 시간(초)
	 * @param   OutResult   카메라 구성
	 */
	virtual void CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult);
	
	/**
	 * 액터의 시점을 반환합니다.
	 * 이는 카메라가 아닌, 액터의 '눈'을 의미합니다.
	 * 예를 들어, Pawn의 경우 이는 눈 높이 위치와
	 * 뷰 회전을 정의합니다 (Pawn 회전과는 다릅니다. Pawn 회전의 pitch는 0으로 초기화됩니다).
	 * 1인칭 뷰 카메라는 일반적으로 이 시점을 사용합니다. 대부분의 트레이스(무기, AI)는 이 시점에서 수행됩니다.
	 *
	 * @param   OutLocation - 시점의 위치
	 * @param   OutRotation - 액터의 뷰 회전.
	 */
	//UFUNCTION(BlueprintCallable, Category = Actor)
	virtual void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const;

public:
	AActor(const FObjectInitializer& ObjectInitializer);
	virtual void OnPropertyChanged(struct FProperty&);

public:
	/** bHasRegisteredAllComponents를 반환합니다. 이 값은 이 액터가 모든 컴포넌트를 등록했는지 여부를 나타냅니다.
	 * bHasRegisteredAllComponents는 PostRegisterAllComponents()가 호출되기 전에 true로 설정되고, PostUnregisterAllComponents()가 호출되기 전에 false로 설정됩니다.
	 */
	bool HasActorRegisteredAllComponents() const { return bHasRegisteredAllComponents; }

public:
	template<class T>
	static FORCEINLINE FRotator TemplateGetActorRotation(const T* RootComponent)
	{
		return (RootComponent != nullptr) ? RootComponent->GetComponentRotation() : FRotator::ZeroRotator;
	}

	template<class T>
	static FORCEINLINE FVector TemplateGetActorLocation(const T* RootComponent)
	{
		return (RootComponent != nullptr) ? RootComponent->GetComponentLocation() : FVector::Zero;
	}

	template<class T>
	static FORCEINLINE FVector TemplateGetActorScale(const T* RootComponent)
	{
		return (RootComponent != nullptr) ? RootComponent->GetComponentScale() : FVector(1.f, 1.f, 1.f);
	}

	/** 이 액터의 RootComponent의 회전을 반환합니다. */
	FORCEINLINE FRotator GetActorRotation() const
	{
		return TemplateGetActorRotation(RootComponent.Get());
	}

	/** 이 액터의 RootComponent의 위치를 반환합니다. */
	FORCEINLINE FVector GetActorLocation() const
	{
		return TemplateGetActorLocation(RootComponent.Get());
	}

	/** 이 액터의 RootComponent의 크기를 반환합니다. */
	FORCEINLINE FVector GetActorScale() const
	{
		return TemplateGetActorScale(RootComponent.Get());
	}

	/**
	 * 액터를 지정된 위치로 즉시 이동시킵니다.
	 *
	 * @param NewLocation   액터를 텔레포트할 새 위치입니다.
	 * @param bSweep        목적지 위치로 이동할 때 장애물에 의해 막히면 중간에 멈추고 겹침을 유발하는지 여부를 결정합니다.
	 *                      루트 컴포넌트만 이동하며 차단 충돌을 검사합니다. 자식 컴포넌트는 이동 시 검사되지 않습니다. 충돌이 꺼져 있으면 이 옵션은 영향을 미치지 않습니다.
	 * @param Teleport      물리 상태를 어떻게 텔레포트할 것인지 결정합니다(이 객체에 대해 물리 충돌이 활성화된 경우).
	 *                      ETeleportType::TeleportPhysics와 같으면 이 객체의 물리 속도는 변경되지 않습니다(따라서 래그돌 부분은 위치 변경의 영향을 받지 않습니다).
	 *                      ETeleportType::None와 같으면 물리 속도는 위치 변경에 따라 업데이트됩니다(래그돌 부분에 영향을 줍니다).
	 *                      CCD가 켜져 있고 텔레포트하지 않으면 전체 이동 볼륨에 영향을 미칩니다.
	 *                      텔레포트할 때는 자식/부착된 컴포넌트도 텔레포트되며, 현재 오프셋을 유지합니다. 시뮬레이션되고 있는 경우에도 마찬가지입니다.
	 *                      텔레포트하지 않고 위치를 설정하면 시뮬레이션된 자식/부착된 컴포넌트의 위치는 업데이트되지 않습니다.
	 * @param OutSweepHitResult 이동 시 겹침이 발생한 경우의 히트 결과입니다.
	 * @return  스위핑되지 않은 경우 위치가 성공적으로 설정되었는지, 스위핑된 경우 이동이 발생했는지 여부를 반환합니다.
	 */
	bool SetActorLocation(const FVector& NewLocation/*, bool bSweep = false, FHitResult* OutSweepHitResult = nullptr, ETeleportType Teleport = ETeleportType::None*/);

public:
	virtual void Save(FArchive& Ar);
	virtual void Load(FArchive& Ar);
	virtual void Serialize(FArchive& Ar);

public:
	void AddOwnedComponent(TObjectPtr<UActorComponent> Component);

public:
	/** Returns this actor's root component. */
	FORCEINLINE USceneComponent* GetRootComponent() const { return RootComponent.Get(); }

	/** Instigator의 값을 설정하지만 이 인스턴스에 다른 부작용을 일으키지 않습니다. */
	void SetInstigator(APawn* InInstigator);

	/**
	 * 이 액터의 소유자를 설정합니다. 주로 네트워크 복제에 사용됩니다.
	 * @param NewOwner  이 액터의 소유권을 가져가는 액터
	 */
	//UFUNCTION(BlueprintCallable, Category = Actor)
	virtual void SetOwner(AActor* NewOwner);

	/** See if this actor is owned by TestOwner. */
	inline bool IsOwnedBy(const AActor* TestOwner) const
	{
		for (const AActor* Arg = this; Arg; Arg = Arg->Owner)
		{
			if (Arg == TestOwner)
				return true;
		}
		return false;
	}
	
	/** 이 액터의 instigator를 반환합니다. instigator가 없으면 nullptr를 반환합니다. */
	//UFUNCTION(BlueprintCallable, meta = (BlueprintProtected = "true"), Category = "Game")
	APawn* GetInstigator() const;

	/**
	 * 특정 클래스 유형으로 변환된 instigator를 가져옵니다.
	 * @return 이 액터의 instigator가 지정된 유형인 경우 instigator를 반환하고, 그렇지 않으면 nullptr를 반환합니다.
	 */
	template <class T>
	T* GetInstigator() const
	{
		return Cast<T>(GetInstigator());
	}


	/** 캐시된 월드 포인터에 대한 게터로, 액터가 실제로 레벨에 생성되지 않은 경우 null을 반환합니다. */
	virtual UWorld* GetWorld() const override final;

	/** Return the ULevel that this Actor is part of. */
	//UFUNCTION(BlueprintCallable, Category = Level)
	ULevel* GetLevel() const;

	/**
	* 지정된 컴포넌트를 루트 컴포넌트로 설정합니다. NewRootComponent의 소유자는 이 액터여야 합니다.
	* @return 성공하면 true를 반환합니다.
	*/
	bool SetRootComponent(USceneComponent* NewRootComponent);

public:
	/** 액터가 월드에 스폰된 후 호출됩니다. 플레이를 위한 액터 설정을 담당합니다. */
	void PostSpawnInitialize(FTransform const& SpawnTransform, AActor* InOwner, APawn* InInstigator/*, bool bRemoteOwned, bool bNoFail, bool bDeferConstruction*/, ESpawnActorScaleMethod TransformScaleMethod = ESpawnActorScaleMethod::MultiplyWithRoot);

	/** Components 배열의 모든 컴포넌트가 등록되었는지 확인합니다. */
	virtual void RegisterAllComponents();

	/** Components 배열의 모든 컴포넌트가 등록되기 전에 호출되며, 에디터와 게임 플레이 중 모두 호출됩니다. */
	virtual void PreRegisterAllComponents();

	/**
	 * 점진적으로 이 액터와 연관된 컴포넌트를 등록합니다. 레벨 스트리밍 중에 사용됩니다.
	 *
	 * @param NumComponentsToRegister 이번 실행에서 등록할 컴포넌트의 수입니다. 0으로 설정하면 모든 컴포넌트를 등록합니다.
	 * @return 이 액터의 모든 컴포넌트가 등록되었을 때 true를 반환합니다.
	 */
	bool IncrementalRegisterComponents(int32 NumComponentsToRegister, FRegisterComponentContext* Context = nullptr);

	/**
	 * Components 배열의 모든 컴포넌트가 등록된 후에 호출되며, 편집기와 게임 플레이 중 모두 호출됩니다.
	 * 이 함수를 호출하기 전에 bHasRegisteredAllComponents는 true로 설정되어야 합니다.
	 */
	virtual void PostRegisterAllComponents();

public:
	/**
	 * 액터가 월드에 스폰된 후(즉, UWorld::SpawnActor로부터), 편집기와 게임 플레이 중 모두 호출됩니다.
	 * 루트 컴포넌트를 가진 액터의 경우, 위치와 회전이 이미 설정되어 있을 것입니다.
	 * 이것은 ConstructionScript를 호출하기 전에 호출되지만, 네이티브 컴포넌트가 생성된 후에 호출됩니다.
	 */
	virtual void PostActorCreated();

	/** 일반적으로 연기된 스포닝의 경우, 스포닝 프로세스를 완료하기 위해 호출됩니다. */
	void FinishSpawning(const FTransform& Transform, bool bIsDefaultTransform = false/*, const FComponentInstanceDataCache* InstanceDataCache = nullptr*/, ESpawnActorScaleMethod TransformScaleMethod = ESpawnActorScaleMethod::OverrideRootScale);

	/**
	 * 이 클래스의 인스턴스가 배치되거나(편집기에서) 스폰될 때 호출됩니다.
	 * @param Transform - 액터가 생성된 변환입니다.
	 */
	virtual void OnConstruction(const FTransform& Transform) {}

	/** 액터의 생성이 완료된 후 호출됩니다. 액터 스폰 프로세스를 마무리하는 역할을 합니다. */
	void PostActorConstruction();

	/** 컴포넌트가 초기화되기 직전에 호출되며, 게임 플레이 중에만 호출됩니다. */
	virtual void PreInitializeComponents();

public:
	/**
	 * 'ComponentType' 클래스에서 파생된 모든 컴포넌트를 가져와 결과를 OutComponents 배열에 채웁니다.
	 * 메모리 할당 비용을 피하기 위해 TInlineAllocator를 사용하는 TArrays를 사용하는 것이 좋습니다.
	 * TInlineComponentArray는 이를 쉽게 하기 위해 정의되어 있습니다. 예를 들어:
	 * {
	 *     TInlineComponentArray<UPrimitiveComponent*> PrimComponents(Actor);
	 * }
	 *
	 * @param bIncludeFromChildActors true인 경우 ChildActor 컴포넌트로 재귀하여 해당 액터의 적절한 유형의 컴포넌트를 찾습니다.
	 */
	template<class ComponentType>
	void GetComponents(TArray<ComponentType*>& OutComponents/*, bool bIncludeFromChildActors = false*/) const
	{
		OutComponents.clear();

		TSubclassOf<UActorComponent> ComponentClass = ComponentType::StaticClass();
		for (TObjectPtr<UActorComponent> OwnedComponent : OwnedComponents)
		{
			if (OwnedComponent)
			{
				if (OwnedComponent->IsA(ComponentClass))
				{
					OutComponents.push_back((ComponentType *)OwnedComponent.get());
				}
			}
			else
			{
				E_LOG(Error, TEXT("OwnedComponent is nullptr"));
			}
		}
	}

	/**
	 * null 포인터가 제거된 복사본이 아닌 Components 집합에 대한 직접 참조를 가져옵니다.
	 * 경고: 컴포넌트의 소유권 변경 또는 파괴를 유발할 수 있는 모든 작업은 이 배열을 무효화하므로,
	 * 이 집합을 반복할 때 주의하십시오!
	 */
	const TArray<TObjectPtr<UActorComponent>>& GetComponents() const
	{
		return OwnedComponents;
	}

	/** 컴포넌트가 올바르게 소유자의 OwnedComponents 배열에 포함되어 있는지 확인하는 유틸리티 함수 */
	bool OwnsComponent(UActorComponent* Component) const;

public:
	/**
	 * 액터에서 월드로 변환을 가져옵니다.
	 * @return 액터 공간에서 월드 공간으로 변환하는 변환입니다.
	 */
	//UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Actor Transform", ScriptName = "GetActorTransform"), Category = "Transformation")
	const FTransform& GetTransform() const
	{
		return ActorToWorld();
	}

	/** RootComponent의 로컬에서 월드로 변환을 가져옵니다. GetTransform()과 동일합니다. */
	FORCEINLINE const FTransform& ActorToWorld() const
	{
		return (RootComponent ? RootComponent->GetComponentTransform() : FTransform::Identity);
	}

public:
	/**
	 * 이 액터에서 매 프레임마다 호출되는 함수입니다. 매 프레임마다 실행할 사용자 정의 로직을 구현하려면 이 함수를 재정의하세요.
	 * 기본적으로 Tick은 비활성화되어 있으며, 이를 활성화하려면 PrimaryActorTick.bCanEverTick이 true로 설정되어 있는지 확인해야 합니다.
	 *
	 * @param   DeltaSeconds    시간 확장에 의해 수정된 마지막 프레임 동안 경과된 게임 시간
	 */
	virtual void Tick(float DeltaSeconds);

public:
	/** 이 액터에 대해 BeginPlay 호출을 시작합니다. 올바른 순서로 호출할 것을 처리합니다. */
	void DispatchBeginPlay();

	/** 액터에 대해 BeginPlay가 호출되었는지 (그리고 EndPlay이 호출되지 않았는지)를 반환합니다. */
	bool HasActorBegunPlay() const { return ActorHasBegunPlay == EActorBeginPlayState::HasBegunPlay; }

	/** 게임 플레이를 위해 액터가 초기화되었는지 여부를 반환합니다. */
	bool IsActorInitialized() const { return bActorInitialized; }

	/** 컴포넌트 배열을 반복하고 InitializeComponent를 호출합니다. 이는 한 번에 한 액터에 대해 발생합니다. */
	void InitializeComponents();

	/** 게임 플레이 중에만 호출되며, 모든 컴포넌트가 초기화된 후 C++ 쪽에서 액터가 자신을 초기화할 수 있도록 합니다. */
	virtual void PostInitializeComponents();

protected:
	/** 이 액터의 플레이가 시작될 때 발생하는 가상 네이티브 이벤트입니다. */
	virtual void BeginPlay();

public:
	/**
	 * 이 액터가 BeginPlay 이벤트를 받기 전에 Tick을 허용할지 여부입니다.
	 * 일반적으로 우리는 BeginPlay 이후에만 액터를 Tick합니다; 이 설정을 통해 이러한 동작을 무효화할 수 있습니다.
	 * 이 설정이 관련되기 위해서는 이 액터가 Tick할 수 있어야 합니다.
	 */
	//UPROPERTY(VisibleAnywhere /*EditDefaultsOnly, Category = Tick*/)
	UPROPERTY(EditAnywhere /*EditDefaultsOnly, Category = Tick*/)
	bool bAllowTickBeforeBeginPlay = false;

	/**
	 *  액터를 게임에서 숨기도록 설정합니다.
	 *  @param  bNewHidden  액터 및 모든 구성 요소를 숨길지 여부입니다.
	 */
	//UFUNCTION(BlueprintCallable, Category = "Rendering", meta = (DisplayName = "Set Actor Hidden In Game", Keywords = "Visible Hidden Show Hide"))
	virtual void SetActorHiddenInGame(bool bNewHidden);
	virtual bool IsHiddenInGame() const { return bHidden; }

public:
	/**
	 * 이 액터의 소유자입니다. 주로 복제(bNetUseOwnerRelevancy 및 bOnlyRelevantToOwner) 및 가시성(PrimitiveComponent bOwnerNoSee 및 bOnlyOwnerSee)에 사용됩니다.
	 * @see SetOwner(), GetOwner()
	 */
	//UPROPERTY(ReplicatedUsing = OnRep_Owner)
	TEnginePtr<AActor> Owner;

	/** 이 액터가 소유자인 모든 액터의 배열, 이들은 반드시 UChildActorComponent에 의해 스폰된 것은 아닙니다 */
	//UPROPERTY(Transient)
	TArray<TEnginePtr<AActor>> Children;

public:
	/** 입력이 활성화된 경우, 이 액터에 대한 입력을 처리하는 컴포넌트입니다. */
	//UPROPERTY(DuplicateTransient)
	TEnginePtr<class UInputComponent> InputComponent;

protected:
	/** 이 액터의 트랜스폼(위치, 회전, 스케일)을 정의하는 컴포넌트로, 모든 다른 컴포넌트는 이 컴포넌트에 어떻게든 부착되어야 합니다 */
	//UPROPERTY(BlueprintGetter = K2_GetRootComponent, Category = "Transformation")
	TEnginePtr<USceneComponent> RootComponent;

private:
	/** 이 액터가 일으킨 피해 및 기타 게임플레이 이벤트에 대한 책임이 있는 폰입니다. */
	//UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_Instigator, meta = (ExposeOnSpawn = true, AllowPrivateAccess = true), Category = Actor)
	TEnginePtr<class APawn> Instigator;

private:
	/**
	 * 이 액터가 소유한 모든 ActorComponent입니다. 액터는 많은 수의 컴포넌트를 가질 수 있으므로 Set으로 저장됩니다.
	 * @see GetComponents()
	 */
	// 우리는 저장 순서 이슈로 TArray
	TArray<TObjectPtr<UActorComponent>> OwnedComponents;

private:
	/** PostRegisterAllComponents()가 호출되기 직전에 true로 설정되고, PostUnregisterAllComponents()가 호출되기 직전에 false로 설정됩니다. */
	uint8 bHasRegisteredAllComponents : 1 = false;
	
	/** 이 액터에 대해 FinishSpawning이 호출되었는지 여부. 호출되지 않은 경우, 액터는 잘못된 상태에 있습니다. */
	uint8 bHasFinishedSpawning : 1 = false;

	/**
	 * PreInitializeComponents/PostInitializeComponents가 이 액터에 대해 호출되었음을 나타냅니다.
	 * 레벨 시작 시 생성된 액터의 재초기화를 방지합니다.
	 */
	uint8 bActorInitialized : 1 = false;

	/**
	 * 이 액터를 실제 게임이 아닌 에디터에서만 볼 수 있게 해줍니다.
	 * @see SetActorHiddenInGame()
	 */
	UPROPERTY(/*Interp, */EditAnywhere/*, Category = Rendering, BlueprintReadOnly, Replicated, meta = (AllowPrivateAccess = "true", DisplayName = "Actor Hidden In Game", SequencerTrackClass = "/Script/MovieSceneTracks.MovieSceneVisibilityTrack")*/)
	bool bHidden = false;

	/** BeginPlay가 시작되었거나 완료되었는지를 나타내는 Enum */
	enum class EActorBeginPlayState : uint8
	{
		HasNotBegunPlay,
		BeginningPlay,
		HasBegunPlay,
	};

	/**
	 * 이 액터에 대해 BeginPlay가 호출되었음을 나타냅니다.
	 * EndPlay이 호출되면 HasNotBegunPlay로 다시 설정됩니다.
	 */
	EActorBeginPlayState ActorHasBegunPlay : 2 = EActorBeginPlayState::HasNotBegunPlay;

//public:
//	UPROPERTY(VisibleAnywhere)
//	int Value = 0;
//
//	UPROPERTY(EditAnywhere)
//	int Value2 = 1;
};