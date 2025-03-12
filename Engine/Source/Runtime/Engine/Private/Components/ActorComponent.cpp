#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "SceneInterface.h"
#include "Components/PrimitiveComponent.h"

ENGINE_API void FRegisterComponentContext::SendRenderDynamicData(FRegisterComponentContext* Context, UPrimitiveComponent* PrimitiveComponent)
{
}

void FRegisterComponentContext::Process()
{
	FSceneInterface* Scene = World->Scene;

	for (uint64 Index = 0; Index < AddPrimitiveBatches.size(); ++Index)
	{
		UPrimitiveComponent* Component = AddPrimitiveBatches[Index];

		// AActor::PostRegisterAllComponents (IncrementalRegisterComponents에서 호출됨)은 컴포넌트를 제거하거나 다시 등록하는 코드를 실행할 수 있습니다.
		// 제거되면 이 컴포넌트를 건너뜁니다. 다시 등록되면 FRegisterComponentContext가 전달되지 않으므로 SceneProxy를 생성할 수 있습니다.
		if (Component && Component->IsRegistered())
		{
			if (Component->IsRenderStateCreated()/* || !bAppCanEverRender*/)
			{
				// SceneProxy가 이미 생성되었으면 건너뜁니다.
				if (Component->SceneProxy == nullptr)
				{
					Scene->AddPrimitive(Component);
				}
			}
			else // 컴포넌트 렌더 상태가 누락된 경우의 경계값을 위한 후처리입니다.
			{
				_ASSERT(false); // 왜 들어왔나 확인 해보기
				Component->CreateRenderState_Concurrent(nullptr);
			}
		}
	}

	AddPrimitiveBatches.clear();
}

UActorComponent::UActorComponent()
{
    OwnerPrivate = GetTypedOuter<AActor>();
}

void UActorComponent::PostInitProperties()
{
	Super::PostInitProperties();

	if (OwnerPrivate)
	{
		OwnerPrivate->AddOwnedComponent(As<UActorComponent>());
	}
}

AActor* UActorComponent::GetOwner() const
{
    return GetActorOwnerNoninline();
}

void UActorComponent::RegisterComponent()
{
	AActor* MyOwner = GetOwner();
	UWorld* MyOwnerWorld = (MyOwner ? MyOwner->GetWorld() : nullptr);
	_ASSERT(MyOwnerWorld);
	if (MyOwnerWorld)
	{
		//@note FH: world should be initialized when calling RegisterComponent or it should be handled gracefully as a no-op but that isn't currently the case
		// however a lot of legacy code may end up calling RegisterComponent prior to world initialization hence why the ensure is currently commented
		//ensure(MyOwnerWorld->bIsWorldInitialized);
		RegisterComponentWithWorld(MyOwnerWorld);
	}
}

void UActorComponent::RegisterComponentWithWorld(UWorld* InWorld, FRegisterComponentContext* Context)
{
	if (!this)
	{
		E_LOG(Log, TEXT("RegisterComponentWithWorld: ({}) Trying to register component with IsValid() == false. Aborting."), GetName());
		return;
	}

	// If the component was already registered, do nothing
	if (IsRegistered())
	{
		E_LOG(Log, TEXT("RegisterComponentWithWorld: ({}) Already registered. Aborting."), GetName());
		return;
	}

	if (InWorld == nullptr)
	{
		//UE_LOG(LogActorComponent, Log, TEXT("RegisterComponentWithWorld: (%s) NULL InWorld specified. Aborting."), *GetPathName());
		return;
	}

	// 등록되지 않은 경우, 씬을 가져서는 안 됩니다.
	_ASSERT(WorldPrivate == nullptr);// , TEXT("%s"), * GetFullName());

	AActor* MyOwner = GetOwner();
	_ASSERT(MyOwner == nullptr || MyOwner->OwnsComponent(this));

	if (!bHasBeenCreated)
	{
		OnComponentCreated();
	}

	WorldPrivate = InWorld;

	ExecuteRegisterEvents(Context);
}

void UActorComponent::OnComponentCreated()
{
	_ASSERT(!bHasBeenCreated);
	bHasBeenCreated = true;
}

void UActorComponent::ExecuteRegisterEvents(FRegisterComponentContext* Context)
{
	if (!bRegistered)
	{
		OnRegister();

		if (!bRegistered)
		{
			E_LOG(Error, TEXT("Failed to route OnRegister ({})"), GetName());
		}
	}

	if (/*FApp::CanEverRender() &&*/ !bRenderStateCreated && WorldPrivate->Scene && ShouldCreateRenderState())
	{
		CreateRenderState_Concurrent(Context);
		if (!bRenderStateCreated)
		{
			E_LOG(Error, TEXT("Failed to route CreateRenderState_Concurrent ({})"), GetName());
		}
	}

	// CreatePhysicsState(/*bAllowDeferral=*/true);
}

void UActorComponent::OnRegister()
{
	_ASSERT(!bRegistered);
	bRegistered = true;

	UpdateComponentToWorld();

	if (bAutoActivate)
	{
		AActor* Owner = GetOwner();
		if (!WorldPrivate->IsGameWorld() || Owner == nullptr || Owner->IsActorInitialized())
		{
			Activate(true);
		}
	}
}

void UActorComponent::CreateRenderState_Concurrent(FRegisterComponentContext* Context)
{
	_ASSERT(IsRegistered());
	_ASSERT(WorldPrivate->Scene);
	_ASSERT(!bRenderStateCreated);
	bRenderStateCreated = true;

	/*bRenderStateDirty = false;
	bRenderTransformDirty = false;
	bRenderDynamicDataDirty = false;
	bRenderInstancesDirty = false;

#if LOG_RENDER_STATE
	UE_LOG(LogActorComponent, Log, TEXT("CreateRenderState_Concurrent: %s"), *GetPathName());
#endif

#if WITH_EDITOR
	FObjectCacheEventSink::NotifyRenderStateChanged_Concurrent(this);
#endif*/
}

void UActorComponent::Activate(bool bReset)
{
	if (bReset || ShouldActivate() == true)
	{
		SetComponentTickEnabled(true);
		SetActiveFlag(true);

		//OnComponentActivated.Broadcast(this, bReset);
	}
}

void UActorComponent::SetComponentTickEnabled(bool bEnabled)
{
	// @TODO
	// 이것도 해야함 RegisterActorTickFunctions
}

void UActorComponent::InitializeComponent()
{
	_ASSERT(bRegistered);
	_ASSERT(!bHasBeenInitialized);

	bHasBeenInitialized = true;
}

void UActorComponent::BeginPlay()
{
	_ASSERT(bRegistered);
	_ASSERT(!bHasBegunPlay);
	//_ASSERT(bTickFunctionsRegistered); // If this fails, someone called BeginPlay() without first calling RegisterAllComponentTickFunctions().

	/*if (GetClass()->HasAnyClassFlags(CLASS_CompiledFromBlueprint) || !GetClass()->HasAnyClassFlags(CLASS_Native))
	{
		ReceiveBeginPlay();
	}*/

	bHasBegunPlay = true;
}

void UActorComponent::SetActiveFlag(const bool bNewIsActive)
{
	bIsActive = bNewIsActive;
}

bool UActorComponent::ShouldActivate() const
{
	// 활성 상태가 아닌 경우, 활성화해야 합니다.
	return !IsActive();
}

AActor* UActorComponent::GetActorOwnerNoninline() const
{
    _ASSERT(OwnerPrivate);
    return OwnerPrivate;
}
