#include "GameFramework/Actor.h"
#include "Camera/CameraTypes.h"
#include "Engine/World.h"
#include "Engine/Level.h"

/** 사용자가 자신의 네이티브 생성자에서 이를 잊어버렸을 때, 액터의 컴포넌트 계층 구조를 설정하는 유틸리티 */
static USceneComponent* FixupNativeActorComponents(AActor* Actor)
{
	USceneComponent* SceneRootComponent = Actor->GetRootComponent();
	if (SceneRootComponent == nullptr)
	{
		TArray<USceneComponent*> SceneComponents;
		Actor->GetComponents(SceneComponents);
		if (SceneComponents.size() > 0)
		{
			E_LOG(Error, TEXT("{}에는 기본적으로 씬 컴포넌트가 추가되어 있지만, 이들 중 어느 것도 액터의 RootComponent로 설정되지 않았습니다 - 임의로 하나를 선택합니다."), Actor->GetName());

			// 사용자가 네이티브 컴포넌트 중 하나를 루트로 설정하는 것을 잊어버렸다면,
			// 임의로 하나를 선택합니다 (그렇지 않으면 SCS가 자체 루트를 생성하고, 네이티브 컴포넌트를 그 아래에 중첩할 수 있습니다).
			for (USceneComponent* Component : SceneComponents)
			{
				/// @TODO
				//if ((Component == nullptr) ||
				//	(Component->GetAttachParent() != nullptr))// ||
				//	//(Component->CreationMethod != EComponentCreationMethod::Native))
				//{
				//	continue;
				//}

				SceneRootComponent = Component;
				Actor->SetRootComponent(Component);
				break;
			}
		}
	}

	return SceneRootComponent;
}

void AActor::CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult)
{
	//if (bFindCameraComponentWhenViewTarget)
	//{
	//	// Look for the first active camera component and use that for the view
	//	TInlineComponentArray<UCameraComponent*> Cameras;
	//	GetComponents(/*out*/ Cameras);

	//	for (UCameraComponent* CameraComponent : Cameras)
	//	{
	//		if (CameraComponent->IsActive())
	//		{
	//			CameraComponent->GetCameraView(DeltaTime, OutResult);
	//			return;
	//		}
	//	}
	//}

	GetActorEyesViewPoint(OutResult.Location, OutResult.Rotation);
}

void AActor::GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const
{
	OutLocation = GetActorLocation();
	OutRotation = GetActorRotation();
}

AActor::AActor(const FObjectInitializer& ObjectInitializer)
{

}

void AActor::OnPropertyChanged(FProperty& InProperty)
{
	Super::OnPropertyChanged(InProperty);
	for (TEnginePtr<UActorComponent> It : OwnedComponents)
	{
		It->OnPropertyChanged(InProperty);
	}
}

bool AActor::SetActorLocation(const FVector& NewLocation)
{
	if (RootComponent)
	{
		//const FVector Delta = NewLocation - GetActorLocation();
		//return RootComponent->MoveComponent(Delta, GetActorQuat(), bSweep, OutSweepHitResult, MOVECOMP_NoFlags, Teleport);
		RootComponent->SetWorldLocation(NewLocation);
		return true;
	}
	//else if (OutSweepHitResult)
	//{
	//	*OutSweepHitResult = FHitResult();
	//}

	return false;
}

void AActor::Save(FArchive& Ar)
{
	uint64 ComponentSize = OwnedComponents.size();
	Ar << ComponentSize;

	for (TEnginePtr<UActorComponent> It : OwnedComponents)
	{
		It->Serialize(Ar);
	}
}

void AActor::Load(FArchive& Ar)
{
	uint64 ComponentSize = 0;
	Ar << ComponentSize;

	for (uint64 i = 0; i < ComponentSize; ++i)
	{
		FString ComponentName;
		Ar << ComponentName;

		auto It = find_if(OwnedComponents.begin(), OwnedComponents.end(),
			[&ComponentName](TEnginePtr<UActorComponent> InComponent)
			{
				return InComponent->GetName() == ComponentName;
			});
		if (It != OwnedComponents.end())
		{
			It->get()->Serialize(Ar);
		}
	}
}

void AActor::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	if (Ar.IsSaving())
	{
		uint64 ComponentSize = OwnedComponents.size();
		Ar << ComponentSize;

		for (TEnginePtr<UActorComponent> It : OwnedComponents)
		{
			It->Serialize(Ar);
		}
	}
	else
	{
		uint64 ComponentSize = 0;
		Ar << ComponentSize;

		if (Ar.IsPersistent())
		{
			for (uint64 i = 0; i < ComponentSize; ++i)
			{
				FString ComponentName;
				Ar << ComponentName;

				auto It = find_if(OwnedComponents.begin(), OwnedComponents.end(),
					[&ComponentName](TEnginePtr<UActorComponent> InComponent)
					{
						return InComponent->GetName() == ComponentName;
					});
				if (It != OwnedComponents.end())
				{
					It->get()->Serialize(Ar);
				}
			}
		}
		else
		{
			for (TObjectPtr<UActorComponent> It : OwnedComponents)
			{
				It->Serialize(Ar);
			}
		}
	}
}

void AActor::AddOwnedComponent(TObjectPtr<UActorComponent> Component)
{
	_ASSERT(Component->GetOwner() == this);

	/*if (OwnedComponents.contains(Component))
	{
		_ASSERT(false);
	}*/

	OwnedComponents.push_back(Component);
}

void AActor::PostSpawnInitialize(FTransform const& UserSpawnTransform, AActor* InOwner, APawn* InInstigator, ESpawnActorScaleMethod TransformScaleMethod)
{
	// 일반적인 흐름은 다음과 같습니다
	// - 액터가 기본 설정을 구성합니다.
	// - 액터가 PreInitializeComponents()를 호출합니다.
	// - 액터가 자체적으로 구성되며, 이 시점에서 컴포넌트가 완전히 조립되어야 합니다.
	// - 액터의 컴포넌트가 OnComponentCreated를 호출합니다.
	// - 액터의 컴포넌트가 InitializeComponent를 호출합니다.
	// - 모든 설정이 완료되면 액터가 PostInitializeComponents()를 호출합니다.
	//
	// 이는 지연된 스폰이든 지연되지 않은 스폰이든 동일한 순서여야 합니다.

	// 월드 정보가 스폰되기 전까지는 UWorld 접근자 함수 호출이 안전하지 않습니다.

	// Set owner.
	SetOwner(InOwner);

	// Set instigator
	SetInstigator(InInstigator);

	// 네이티브 루트 컴포넌트가 있는 경우 액터의 월드 트랜스폼을 설정합니다.
	USceneComponent* const SceneRootComponent = FixupNativeActorComponents(this);
	if (SceneRootComponent != nullptr)
	{
		_ASSERT(SceneRootComponent->GetOwner() == this);

		// Respect any non-default transform value that the root component may have received from the archetype that's owned
		// by the native CDO, so the final transform might not always necessarily equate to the passed-in UserSpawnTransform.
		const FTransform RootTransform(SceneRootComponent->GetRelativeRotation(), SceneRootComponent->GetRelativeLocation(), SceneRootComponent->GetRelativeScale3D());
		FTransform FinalRootComponentTransform = RootTransform;
		switch (TransformScaleMethod)
		{
		case ESpawnActorScaleMethod::OverrideRootScale:
			FinalRootComponentTransform = UserSpawnTransform;
			break;
		case ESpawnActorScaleMethod::MultiplyWithRoot:
		case ESpawnActorScaleMethod::SelectDefaultAtRuntime:
			FinalRootComponentTransform = RootTransform * UserSpawnTransform;
			break;
		}
		SceneRootComponent->SetWorldTransform(FinalRootComponentTransform/*, false, nullptr, ETeleportType::ResetPhysics*/);
	}

	// 모든 기본(네이티브) 컴포넌트에서 OnComponentCreated를 호출합니다.
	//DispatchOnComponentsCreated(this);

	// 액터의 기본(네이티브) 컴포넌트를 등록합니다. 단, 네이티브 씬 루트가 있는 경우에만 등록합니다. 네이티브 씬 루트가 없는 경우, 
	// 네이티브 클래스 수준에서 씬 컴포넌트가 없을 수 있음을 의미합니다. 이 경우, 만약 이 인스턴스가 블루프린트 인스턴스라면, 
	// SCS 실행 후 씬 루트를 설정할 수 있을 때까지 네이티브 등록을 연기해야 합니다.
	// 참고: 이 API는 액터 인스턴스에서 PostRegisterAllComponents()를 호출합니다. 연기된 경우, PostRegisterAllComponents()는 루트가 SCS에 의해 설정될 때까지 호출되지 않습니다.
	//bHasDeferredComponentRegistration = (SceneRootComponent == nullptr && Cast<UBlueprintGeneratedClass>(GetClass()) != nullptr);
	if (/*!bHasDeferredComponentRegistration && */GetWorld())
	{
		RegisterAllComponents();
	}

	PostActorCreated();
	
	// 네이티브 및 BP Construction Script를 실행합니다.
	// 이후, 모든 컴포넌트가 생성되고 조립되었음을 가정할 수 있습니다.
	//if (!bDeferConstruction)
	{
		FinishSpawning(UserSpawnTransform, true);
	}
}

void AActor::SetInstigator(APawn* InInstigator)
{
	//Instigator = InInstigator;
}

void AActor::SetOwner(AActor* NewOwner)
{
	if (Owner != NewOwner /*&& IsValidChecked(this)*/)
	{
		if (NewOwner != nullptr && NewOwner->IsOwnedBy(this))
		{
			E_LOG(Error, TEXT("SetOwner(): Failed to set '{}' owner of '{}' because it would cause an Owner loop"), NewOwner->GetName(), GetName());
			return;
		}

		// 이 액터의 부모를 지정된 액터로 설정합니다.
		if (Owner != nullptr)
		{
			// 이전 소유자의 Children 배열에서 제거합니다.
			TEnginePtr<AActor> ThisActor = this->As<AActor>();
			auto ItFind = find(Children.begin(), Children.end(), ThisActor);
			Owner->Children.erase(ItFind);
		}

		Owner = NewOwner->As<AActor>();

		if (Owner != nullptr)
		{
			// 새로운 소유자의 Children 배열에 추가합니다.
			Owner->Children.push_back(this->As<AActor>());
		}
	}
}

APawn* AActor::GetInstigator() const
{
	return Instigator;
}

UWorld* AActor::GetWorld() const
{
	// CDO 객체는 월드에 속하지 않습니다
	// 액터의 Outer가 파괴되었거나 접근할 수 없는 경우, 종료 중이며 월드는 nullptr이어야 합니다
	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		if (ULevel* Level = GetLevel())
		{
			return Level->OwningWorld.Get();
		}
	}
	return nullptr;
}

ULevel* AActor::GetLevel() const
{
	return GetTypedOuter<ULevel>();
}

bool AActor::SetRootComponent(USceneComponent* NewRootComponent)
{
	if (RootComponent != NewRootComponent)
	{
		//USceneComponent* OldRootComponent = RootComponent;
		RootComponent = NewRootComponent->As<USceneComponent>();

		// Notify new root first, as it probably has no delegate on it.
		/*if (NewRootComponent)
		{
			NewRootComponent->NotifyIsRootComponentChanged(true);
		}

		if (OldRootComponent)
		{
			OldRootComponent->NotifyIsRootComponentChanged(false);
		}*/

		return true;
	}

	return false;
}

void AActor::RegisterAllComponents()
{
	PreRegisterAllComponents();

	// 0 - means register all components
	bool bAllRegistered = IncrementalRegisterComponents(0);
	_ASSERT(bAllRegistered);
}

void AActor::PreRegisterAllComponents()
{
}

// Walks through components hierarchy and returns closest to root parent component that is unregistered
// Only for components that belong to the same owner
static USceneComponent* GetUnregisteredParent(UActorComponent* Component)
{
	USceneComponent* ParentComponent = nullptr;
	USceneComponent* SceneComponent = dynamic_cast<USceneComponent*>(Component);

	while (SceneComponent &&
		SceneComponent->GetAttachParent() &&
		SceneComponent->GetAttachParent()->GetOwner() == Component->GetOwner() &&
		!SceneComponent->GetAttachParent()->IsRegistered())
	{
		SceneComponent = SceneComponent->GetAttachParent();
		if (/*SceneComponent->bAutoRegister && IsValidChecked*/(SceneComponent))
		{
			// We found unregistered parent that should be registered
			// But keep looking up the tree
			ParentComponent = SceneComponent;
		}
	}

	return ParentComponent;
}


bool AActor::IncrementalRegisterComponents(int32 NumComponentsToRegister, FRegisterComponentContext* Context)
{
	if (NumComponentsToRegister == 0)
	{
		// 0 - means register all components
		NumComponentsToRegister = std::numeric_limits<int32>::max();
	}

	UWorld* const World = GetWorld();
	_ASSERT(World);

	// 게임 월드가 아닌 경우, 지금 Tick 함수를 등록합니다. 게임 월드인 경우, BeginPlay() 바로 직전에 등록하여
	// BeginPlay()가 실행되기 전에는 실제로 Tick하지 않도록 합니다(네트워크 게임에서는 다르게 작동할 수 있음).
	if (bAllowTickBeforeBeginPlay || !World->IsGameWorld())
	{
		//RegisterAllActorTickFunctions(true, false); // 컴포넌트는 등록될 때 처리됩니다.
	}

	// RootComponent를 먼저 등록하여 다른 모든 자식 컴포넌트가 등록 시 신뢰성 있게 사용할 수 있도록 합니다 (예: GetLocation 호출).
	if (RootComponent != nullptr && !RootComponent->IsRegistered())
	{
		//if (RootComponent->bAutoRegister)
		{
			// 컴포넌트를 등록하기 전에 트랜잭션 버퍼에 저장하여 "취소" 시 비등록 상태로 되돌아가도록 합니다.
			// 이렇게 하면 복사/붙여넣기 또는 복제 작업을 취소할 때 원하지 않는 컴포넌트가 남아 있는 것을 방지할 수 있습니다.
			//RootComponent->Modify(false);

			RootComponent->RegisterComponentWithWorld(World, Context);
		}
	}

	int32 NumTotalRegisteredComponents = 0;
	int32 NumRegisteredComponentsThisRun = 0;
	TArray<UActorComponent*> Components;
	GetComponents(Components);
	TSet<UActorComponent*> RegisteredParents;

	for (int32 CompIdx = 0; CompIdx < Components.size() && NumRegisteredComponentsThisRun < NumComponentsToRegister; CompIdx++)
	{
		UActorComponent* Component = Components[CompIdx];
		if (!Component->IsRegistered() /*&& Component->bAutoRegister && IsValidChecked(Component)*/)
		{
			// Ensure that all parent are registered first
			USceneComponent* UnregisteredParentComponent = GetUnregisteredParent(Component);
			if (UnregisteredParentComponent)
			{
				bool bParentAlreadyHandled = RegisteredParents.contains(UnregisteredParentComponent);
				RegisteredParents.emplace(UnregisteredParentComponent);
				if (bParentAlreadyHandled)
				{
					E_LOG(Error, TEXT("AActor::IncrementalRegisterComponents parent component '{}' cannot be registered in actor '{}'"),
						UnregisteredParentComponent->GetName(), GetName());
					break;
				}

				// Register parent first, then return to this component on a next iteration
				Component = UnregisteredParentComponent;
				CompIdx--;
				NumTotalRegisteredComponents--; // because we will try to register the parent again later...
			}

			// 컴포넌트를 등록하기 전에 트랜잭션 버퍼에 저장하여 "취소" 시 비등록 상태로 되돌아가도록 합니다.
			// 이렇게 하면 복사/붙여넣기 또는 복제 작업을 취소할 때 원하지 않는 컴포넌트가 남아 있는 것을 방지할 수 있습니다.
			//Component->Modify(false);

			Component->RegisterComponentWithWorld(World, Context);
			NumRegisteredComponentsThisRun++;
		}

		NumTotalRegisteredComponents++;
	}

	if (Components.size() == NumTotalRegisteredComponents)
	{
		// 최적화가 활성화되고 이미 호출된 경우 월드 포스트 등록을 건너뜁니다
		//const bool bCallWorldPostRegister = (!bHasRegisteredAllComponents /*|| GOptimizeActorRegistration == 0*/);

		// 더 이상 연기되지 않으므로 이 플래그를 초기화합니다
		//bHasDeferredComponentRegistration = false;

		bHasRegisteredAllComponents = true;
		// 마지막으로 PostRegisterAllComponents를 호출합니다
		PostRegisterAllComponents();

		//if (bCallWorldPostRegister)
		//{
		//	// 모든 컴포넌트가 등록된 후 배우는 완전히 추가된 것으로 간주됩니다: 소유 세계에 알립니다.
		//	PlayWorld->NotifyPostRegisterAllActorComponents(this);
		//}
		return true;
	}
	_ASSERT(false);
	return false;
}

void AActor::PostRegisterAllComponents()
{
}

void AActor::PostActorCreated()
{
}

void AActor::FinishSpawning(const FTransform& UserTransform, bool bIsDefaultTransform, ESpawnActorScaleMethod TransformScaleMethod)
{
	_ASSERT(!bHasFinishedSpawning);
	if (!bHasFinishedSpawning)
	{
		bHasFinishedSpawning = true;

		FTransform FinalRootComponentTransform = (RootComponent ? RootComponent->GetComponentTransform() : UserTransform);

		// Transform을 조정할 필요가 있는지 확인합니다
		// (즉, 원래 SpawnActor 호출 시 전달된 것과 다른 변환이 여기에서 호출자가 전달하는 경우)
		//if (RootComponent && !bIsDefaultTransform)
		//{
		//	FTransform const* const OriginalSpawnTransform = GSpawnActorDeferredTransformCache.Find(this);
		//	if (OriginalSpawnTransform)
		//	{
		//		GSpawnActorDeferredTransformCache.Remove(this);

		//		if (OriginalSpawnTransform->Equals(UserTransform) == false)
		//		{
		//			UserTransform.GetLocation().DiagnosticCheckNaN(TEXT("AActor::FinishSpawning: UserTransform.GetLocation()"));
		//			UserTransform.GetRotation().DiagnosticCheckNaN(TEXT("AActor::FinishSpawning: UserTransform.GetRotation()"));

		//			// caller passed a different transform!
		//			// undo the original spawn transform to get back to the template transform, so we can recompute a good
		//			// final transform that takes into account the template's transform
		//			FTransform const TemplateTransform = RootComponent->GetComponentTransform() * OriginalSpawnTransform->Inverse();
		//			FinalRootComponentTransform = TemplateTransform * UserTransform;
		//		}
		//	}

		//	// should be fast and relatively rare
		//	ValidateDeferredTransformCache();
		//}

		//FinalRootComponentTransform.GetLocation().DiagnosticCheckNaN(TEXT("AActor::FinishSpawning: FinalRootComponentTransform.GetLocation()"));
		//FinalRootComponentTransform.GetRotation().DiagnosticCheckNaN(TEXT("AActor::FinishSpawning: FinalRootComponentTransform.GetRotation()"));

		{
			//ExecuteConstruction(FinalRootComponentTransform, nullptr, InstanceDataCache, bIsDefaultTransform, TransformScaleMethod);
			OnConstruction(FinalRootComponentTransform);
		}

		{
			PostActorConstruction();
		}
	}
}

void AActor::PostActorConstruction()
{
	UWorld* const World = GetWorld();
	bool const bActorsInitialized = World && World->AreActorsInitialized();

	if (bActorsInitialized)
	{
		PreInitializeComponents();
	}

	// 이 액터가 동적으로 스폰된 복제된(Replicated) 액터인 경우, 복제된 속성이 역직렬화될 때까지 BeginPlay 및 UpdateOverlaps 호출을 연기합니다.
	//const bool bDeferBeginPlayAndUpdateOverlaps = (bExchangedRoles && RemoteRole == ROLE_Authority) && !GIsReinstancing;

	if (bActorsInitialized)
	{
		// Call InitializeComponent on components
		InitializeComponents();

		// 이제 actor의 모든 구성 요소가 생성되고 등록되었으므로, 필요한 충돌 검사 및 처리를 수행합니다.
		if (World)
		{
			//switch (SpawnCollisionHandlingMethod)
			//{
			//case ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn:
			//{
			//	// Try to find a spawn position
			//	FVector AdjustedLocation = GetActorLocation();
			//	FRotator AdjustedRotation = GetActorRotation();
			//	if (World->FindTeleportSpot(this, AdjustedLocation, AdjustedRotation))
			//	{
			//		SetActorLocationAndRotation(AdjustedLocation, AdjustedRotation, false, nullptr, ETeleportType::TeleportPhysics);
			//	}
			//}
			//break;
			//case ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding:
			//{
			//	// Try to find a spawn position			
			//	FVector AdjustedLocation = GetActorLocation();
			//	FRotator AdjustedRotation = GetActorRotation();
			//	if (World->FindTeleportSpot(this, AdjustedLocation, AdjustedRotation))
			//	{
			//		SetActorLocationAndRotation(AdjustedLocation, AdjustedRotation, false, nullptr, ETeleportType::TeleportPhysics);
			//	}
			//	else
			//	{
			//		UE_LOG(LogSpawn, Warning, TEXT("SpawnActor failed because of collision at the spawn location [%s] for [%s]"), *AdjustedLocation.ToString(), *GetClass()->GetName());
			//		Destroy();
			//	}
			//}
			//break;
			//case ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding:
			//	if (World->EncroachingBlockingGeometry(this, GetActorLocation(), GetActorRotation()))
			//	{
			//		UE_LOG(LogSpawn, Warning, TEXT("SpawnActor failed because of collision at the spawn location [%s] for [%s]"), *GetActorLocation().ToString(), *GetClass()->GetName());
			//		Destroy();
			//	}
			//	break;
			//case ESpawnActorCollisionHandlingMethod::Undefined:
			//case ESpawnActorCollisionHandlingMethod::AlwaysSpawn:
			//default:
			//	// note we use "always spawn" as default, so treat undefined as that
			//	// nothing to do here, just proceed as normal
			//	break;
			//}
		}

		if (/*IsValidChecked*/(this))
		{
			PostInitializeComponents();
			if (/*IsValidChecked*/(this))
			{
				if (!bActorInitialized)
				{
					E_LOG(Fatal, TEXT("{} failed to route PostInitializeComponents.  Please call Super::PostInitializeComponents() in your <className>::PostInitializeComponents() function. "), GetName());
				}

				bool bRunBeginPlay = /*!bDeferBeginPlayAndUpdateOverlaps &&*/ (/*BeginPlayCallDepth > 0 ||*/ World->HasBegunPlay());
				if (bRunBeginPlay)
				{
					//if (AActor* ParentActor = GetParentActor())
					//{
					//	// Child Actors cannot run begin play until their parent has run
					//	bRunBeginPlay = (ParentActor->HasActorBegunPlay() || ParentActor->IsActorBeginningPlay());
					//}
				}

//#if WITH_EDITOR
//				if (bRunBeginPlay && bIsEditorPreviewActor)
//				{
//					bRunBeginPlay = false;
//				}
//#endif

				if (bRunBeginPlay)
				{
					//SCOPE_CYCLE_COUNTER(STAT_ActorBeginPlay);
					DispatchBeginPlay();
				}
			}
		}
	}
	else
	{
		// 초기 undo 기록이 만들어질 때 객체가 무효화되도록 하여,
		// 액터가 삭제된 것으로 처리되도록 합니다.
		// 그런 경우 undo 시 추가가 실제로 작동하게 됩니다.
		//MarkAsGarbage();
		//Modify(false);
		//ClearGarbage();
	}
}

void AActor::PreInitializeComponents()
{
	// 특정 경우에만 들어오는 것으로 보임
	/*if (AutoReceiveInput != EAutoReceiveInput::Disabled)
	{
		const int32 PlayerIndex = int32(AutoReceiveInput.GetValue()) - 1;

		APlayerController* PC = UGameplayStatics::GetPlayerController(this, PlayerIndex);
		if (PC)
		{
			EnableInput(PC);
		}
		else
		{
			GetWorld()->PersistentLevel->RegisterActorForAutoReceiveInput(this, PlayerIndex);
		}
	}*/
}

bool AActor::OwnsComponent(UActorComponent* Component) const
{
	TObjectPtr<UActorComponent> ComponentPtr = Component->As<UActorComponent>();
	return find_if(OwnedComponents.begin(), OwnedComponents.end(),
		[Component](TEnginePtr<UActorComponent> InComponent)
		{
			return InComponent == Component;
		}
	) != OwnedComponents.end();
	//return OwnedComponents.contains(ComponentPtr);
}

void AActor::Tick(float DeltaSeconds)
{
}

void AActor::DispatchBeginPlay()
{
	UWorld* World = (!HasActorBegunPlay() && this ? GetWorld() : nullptr);

	if (World)
	{
		//ensureMsgf(ActorHasBegunPlay == EActorBeginPlayState::HasNotBegunPlay, TEXT("BeginPlay was called on actor %s which was in state %d"), *GetPathName(), (int32)ActorHasBegunPlay);
		//const uint32 CurrentCallDepth = BeginPlayCallDepth++;

		//bActorBeginningPlayFromLevelStreaming = bFromLevelStreaming;
		ActorHasBegunPlay = EActorBeginPlayState::BeginningPlay;

		//BuildReplicatedComponentsInfo();

//#if UE_WITH_IRIS
//		BeginReplication();
//#endif // UE_WITH_IRIS

		BeginPlay();

		//ensure(BeginPlayCallDepth - 1 == CurrentCallDepth);
		//BeginPlayCallDepth = CurrentCallDepth;

		//if (bActorWantsDestroyDuringBeginPlay)
		//{
		//	// Pass true for bNetForce as either it doesn't matter or it was true the first time to even 
		//	// get to the point we set bActorWantsDestroyDuringBeginPlay to true
		//	World->DestroyActor(this, true);
		//}

		//if (IsValidChecked(this))
		//{
		//	// Initialize overlap state
		//	UpdateInitialOverlaps(bFromLevelStreaming);
		//}

		//bActorBeginningPlayFromLevelStreaming = false;
	}
}

void AActor::InitializeComponents()
{
	TArray<UActorComponent*> Components;
	GetComponents(Components);

	for (UActorComponent* ActorComp : Components)
	{
		if (ActorComp->IsRegistered())
		{
			if (ActorComp->bAutoActivate && !ActorComp->IsActive())
			{
				ActorComp->Activate(true);
			}

			if (/*ActorComp->bWantsInitializeComponent &&*/ !ActorComp->HasBeenInitialized())
			{
				// Activate가 게임에서 콜백을 발생시키기에 너무 이른 시점에서 일으키므로 활성화 이벤트를 전파합니다.
				ActorComp->InitializeComponent();
			}
		}
	}
}

void AActor::PostInitializeComponents()
{
	bActorInitialized = true;

	// UpdateAllReplicatedComponents();
}

void AActor::BeginPlay()
{
	//SetLifeSpan(InitialLifeSpan);
	//RegisterAllActorTickFunctions(true, false); // Components are done below.

	TArray<UActorComponent*> Components;
	GetComponents(Components);

	for (UActorComponent* Component : Components)
	{
		// bHasBegunPlay이 true일 경우 컴포넌트가 초기화 중에 이름이 변경되고 이동되었습니다.
		if (Component->IsRegistered() && !Component->HasBegunPlay())
		{
			//Component->RegisterAllComponentTickFunctions(true);
			Component->BeginPlay();
			_ASSERT(Component->HasBegunPlay(), TEXT("Failed to route BeginPlay ({})"), Component->GetName());
		}
		else
		{
			// 액터가 플레이를 시작할 때, bAutoRegister가 false인 컴포넌트만 등록되지 않을 것으로 예상됩니다.
			//check(!Component->bAutoRegister);
		}
	}

}

void AActor::SetActorHiddenInGame(bool bNewHidden)
{
	bHidden = bNewHidden;
}
