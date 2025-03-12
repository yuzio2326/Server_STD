#include "Engine/Level.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

ULevel::ULevel()
{
	if (HasAnyFlags(RF_ClassDefaultObject)) { return; }
	OwningWorld = Cast<UWorld>(GetOuter());
}

void ULevel::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	uint64 ActorSize = Ar.IsLoading() ? 0 : Actors.size();
	Ar << ActorSize;

	if (Ar.IsSaving())
	{
		if (Ar.IsPersistent())
		{
			for (TEnginePtr<AActor> Actor : Actors)
			{
				Actor->Serialize(Ar);
			}
		}
		else
		{
			for (TObjectPtr<AActor> Actor : Actors)
			{
				Ar << Actor;
			}
		}
	}
	else
	{
		Actors.reserve(ActorSize);
		for (uint64 i = 0; i < ActorSize; ++i)
		{
			TObjectPtr<AActor> Actor;
			Ar << Actor;
			Actors.push_back(Actor);
		}
	}
}

void ULevel::UpdateLevelComponents(FRegisterComponentContext* Context)
{
	// 모든 컴포넌트를 한 번에 업데이트합니다.
	IncrementalUpdateComponents(0, Context);
}

void ULevel::IncrementalUpdateComponents(int32 NumComponentsToUpdate, FRegisterComponentContext* Context)
{
	bool bFullyUpdateComponents = NumComponentsToUpdate == 0;
	// 편집기는 점진적으로 컴포넌트를 업데이트할 수 없습니다. 모든 액터를 업데이트하려면 NumActorsToUpdate의 값을 0으로 전달하세요.
	_ASSERT(bFullyUpdateComponents || OwningWorld->IsGameWorld());

	do
	{
		switch (IncrementalComponentState)
		{
		case EIncrementalComponentState::Init:
			// 첫 번째 패스에서 BSP를 수행합니다.
			//UpdateModelComponents();
			//// 부모 액터가 자식 액터보다 먼저 등록되도록 액터를 정렬합니다.
			//SortActorsHierarchy(Actors, this);
			IncrementalComponentState = EIncrementalComponentState::RegisterInitialComponents;

			//pass through
		case EIncrementalComponentState::RegisterInitialComponents:
			if (IncrementalRegisterComponents(true, NumComponentsToUpdate, Context))
			{
//#if WITH_EDITOR
//				const bool bShouldRunConstructionScripts = !bHasRerunConstructionScripts && bRerunConstructionScripts && !IsTemplate();
//				IncrementalComponentState = bShouldRunConstructionScripts ? EIncrementalComponentState::RunConstructionScripts : EIncrementalComponentState::Finalize;
//#else
				IncrementalComponentState = EIncrementalComponentState::Finalize;
//#endif
			}
			break;

//#if WITH_EDITOR
//		case EIncrementalComponentState::RunConstructionScripts:
//			if (IncrementalRunConstructionScripts(bFullyUpdateComponents))
//			{
//				IncrementalComponentState = EIncrementalComponentState::Finalize;
//			}
//			break;
//#endif

		case EIncrementalComponentState::Finalize:
			IncrementalComponentState = EIncrementalComponentState::Init;
			//CurrentActorIndexForIncrementalUpdate = 0;
			bHasCurrentActorCalledPreRegister = false;
			bAreComponentsCurrentlyRegistered = true;
			//CreateCluster();
			break;
		}
	} while (bFullyUpdateComponents && !bAreComponentsCurrentlyRegistered);
}

bool ULevel::IncrementalRegisterComponents(bool bPreRegisterComponents, int32 NumComponentsToUpdate, FRegisterComponentContext* Context)
{
	// 다음 유효한 액터를 찾아서 컴포넌트 등록을 처리합니다.

	//if (OwningWorld)
	//{
	//	OwningWorld->SetAllowDeferredPhysicsStateCreation(true);
	//}

	while (CurrentActorIndexForIncrementalUpdate < Actors.size())
	{
		AActor* Actor = Actors[CurrentActorIndexForIncrementalUpdate].get();
		bool bAllComponentsRegistered = true;
		if (/*IsValid*/(Actor))
		{
			if (!Actor->HasActorRegisteredAllComponents() /*|| GOptimizeActorRegistration == 0*/)
			{
//#if PERF_TRACK_DETAILED_ASYNC_STATS
//				FScopeCycleCounterUObject ContextScope(Actor);
//#endif
				if (bPreRegisterComponents && !bHasCurrentActorCalledPreRegister)
				{
					Actor->PreRegisterAllComponents();
					bHasCurrentActorCalledPreRegister = true;
				}
				bAllComponentsRegistered = Actor->IncrementalRegisterComponents(NumComponentsToUpdate, Context);
			}
		}

		if (bAllComponentsRegistered)
		{
			// 이 액터의 모든 컴포넌트가 등록되었습니다. 다음 액터로 이동합니다.
			CurrentActorIndexForIncrementalUpdate++;
			bHasCurrentActorCalledPreRegister = false;
		}

		// 증분 등록을 수행하면 각 처리된 액터 후에 외부 루프로 돌아가서 이 프레임을 계속 처리할지 결정할 수 있도록 합니다.
		if (NumComponentsToUpdate != 0)
		{
			break;
		}
	}

	if (CurrentActorIndexForIncrementalUpdate >= Actors.size())
	{
		// Construction 스크립트를 다시 실행하기 전에 대기 중인 추가를 처리해야 합니다. 이는 내부적으로 제거/추가를 수행할 수 있습니다.
		if (Context)
		{
			Context->Process();
		}
		CurrentActorIndexForIncrementalUpdate = 0;
		return true;
	}

	return false;

}

void ULevel::RouteActorInitialize(int32 NumActorsToProcess)
{
	const bool bFullProcessing = (NumActorsToProcess <= 0);
	switch (RouteActorInitializationState)
	{
	case ERouteActorInitializationState::Preinitialize:
	{
		// 액터의 사전 초기화는 새로운 액터를 생성할 수 있으므로, 액터 수가 안정되도록 점진적으로 처리해야 합니다.
		while (RouteActorInitializationIndex < Actors.size())
		{
			AActor* const Actor = Actors[RouteActorInitializationIndex].get();
			if (Actor && !Actor->IsActorInitialized())
			{
				Actor->PreInitializeComponents();
			}

			++RouteActorInitializationIndex;
			if (!bFullProcessing && (--NumActorsToProcess == 0))
			{
				return;
			}
		}

		RouteActorInitializationIndex = 0;
		RouteActorInitializationState = ERouteActorInitializationState::Initialize;
	}

	// 의도적인 fall-through, 우리의 액터 카운트 예산을 초과하지 않았다면 계속 진행합니다.
	case ERouteActorInitializationState::Initialize:
	{
		while (RouteActorInitializationIndex < Actors.size())
		{
			AActor* const Actor = Actors[RouteActorInitializationIndex].get();
			if (Actor)
			{
				if (!Actor->IsActorInitialized())
				{
					Actor->InitializeComponents();
					Actor->PostInitializeComponents();
					if (!Actor->IsActorInitialized() /*&& IsValidChecked(Actor)*/)
					{
						E_LOG(Fatal, TEXT("{} failed to route PostInitializeComponents. Please call Super::PostInitializeComponents() in your <className>::PostInitializeComponents() function."), Actor->GetName());
					}
				}
			}

			++RouteActorInitializationIndex;
			if (!bFullProcessing && (--NumActorsToProcess == 0))
			{
				return;
			}
		}

		RouteActorInitializationIndex = 0;
		RouteActorInitializationState = ERouteActorInitializationState::BeginPlay;
	}

	// 의도적인 fall-through, 우리의 액터 카운트 예산을 초과하지 않았다면 계속 진행합니다.
	case ERouteActorInitializationState::BeginPlay:
	{
		if (OwningWorld->HasBegunPlay())
		{
			while (RouteActorInitializationIndex < Actors.size())
			{
				// 자식 액터는 부모가 명시적으로 플레이를 시작했습니다.
				AActor* const Actor = Actors[RouteActorInitializationIndex].get();
				if (Actor /*&& !Actor->IsChildActor()*/)
				{
					Actor->DispatchBeginPlay();
				}

				++RouteActorInitializationIndex;
				if (!bFullProcessing && (--NumActorsToProcess == 0))
				{
					return;
				}
			}
		}

		RouteActorInitializationState = ERouteActorInitializationState::Finished;
	}

	// 우리가 끝났다면 의도적인 fall-through
	case ERouteActorInitializationState::Finished:
	{
		break;
	}
	}
}
