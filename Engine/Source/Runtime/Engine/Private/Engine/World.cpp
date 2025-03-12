#include "Engine/World.h"
#include "Engine/EngineBaseTypes.h"
#include "Engine/Level.h"
#include "Engine/Player.h"
#include "EngineModule.h"
#include "RendererInterface.h"
#include "SceneInterface.h"
#include "Engine/GameInstance.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameStateBase.h"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

// Temp
#include "Engine/StaticMeshActor.h"
#include "Engine/DirectionalLight.h"

FDelegate<UWorld*> WorldCreatedDelegate;
FDelegate<UWorld*> WorldDestroyedDelegate;

FActorSpawnParameters::FActorSpawnParameters()
	: Name(NAME_NONE)
	, Template(nullptr)
	, Owner(nullptr)
	, Instigator(nullptr)
	, TransformScaleMethod(ESpawnActorScaleMethod::MultiplyWithRoot)
	, ObjectFlags(EObjectFlags::RF_NoFlags)
{
}

UWorld::UWorld()
{
}

UWorld::~UWorld()
{
	if (HasAnyFlags(EObjectFlags::RF_ClassDefaultObject)) { return; }

	if (Scene)
	{
		Scene->Release();
		Scene = nullptr;
	}

	WorldDestroyedDelegate.Broadcast(this);
}

void UWorld::SetGameState(AGameStateBase* NewGameState)
{
	if (NewGameState == GameState)
	{
		return;
	}

	GameState = NewGameState;
}

void UWorld::InitalizeNewWorld()
{
	PersistentLevel = NewObject<ULevel>(this, ULevel::StaticClass(), TEXT("PersistentLevel"));

	InitWorld();

	FActorSpawnParameters ActorSpawnParameters;
	ActorSpawnParameters.TransformScaleMethod = ESpawnActorScaleMethod::OverrideRootScale;
	FTransform TransformLeft = FTransform(FRotator::ZeroRotator, FVector3D(200.f, -50.f, 0.f), FVector3D(1.f, 1.f, 1.f));
	FTransform TransformRight = FTransform(FRotator::ZeroRotator, FVector3D(200.f, 50.f, 0.f), FVector3D(1.f, 1.f, 1.f));
	AStaticMeshActor* Actor = SpawnActor<AStaticMeshActor>(nullptr, TransformLeft, ActorSpawnParameters);
	AStaticMeshActor* Actor2 = SpawnActor<AStaticMeshActor>(nullptr, TransformRight, ActorSpawnParameters);

	ADirectionalLight* DirectionalLight = SpawnActor<ADirectionalLight>();
	DirectionalLight->SetLightColor(FLinearColor(0.5f, 0.5f, 0.5f));
}

void UWorld::InitializeActorsForPlay(FRegisterComponentContext* Context)
{
	// 한 그룹의 업데이트와 모든 레벨의 구성 요소를 업데이트합니다.
	// 쿠킹된 데이터가 있거나 편집기에서 플레이 중이라면, 빈번히 콜드 데이터를 다시 실행할 필요가 없습니다.
	// 디스크에서 로드되었는지 아니면 복제되었는지에 따라 PIE 세계가 업데이트되었는지 여부를 확인합니다.
	UpdateWorldComponents(Context);

	bActorsInitialized = true;

	// Init the game mode.
	if (AuthorityGameMode && !AuthorityGameMode->IsActorInitialized())
	{
		AuthorityGameMode->InitGame(/*FPaths::GetBaseFilename(InURL.Map), Options, Error*/);
	}

	// 여러 초기화 함수를 라우팅하고 볼륨을 설정합니다.
	const int32 ProcessAllRouteActorInitializationGranularity = 0;
	PersistentLevel->RouteActorInitialize(ProcessAllRouteActorInitializationGranularity);
}

void UWorld::UpdateWorldComponents(FRegisterComponentContext* Context)
{
	PersistentLevel->UpdateLevelComponents(Context);
}

void UWorld::InitWorld()
{
#if !SERVER
	GetRendererModule().AllocateScene(this, ERHIFeatureLevel::SM5);
#endif

	WorldCreatedDelegate.Broadcast(this);
}

void UWorld::Tick(float DeltaSeconds)
{
	if (WorldType == EWorldType::PIE)
	{
		// 우리는 PC가 하나라 여기서 찾겠다
		APlayerController* PlayerController = nullptr;

		for (TObjectPtr<AActor> Actor : PersistentLevel->Actors)
		{
			Actor->Tick(DeltaSeconds);
			if (Actor->GetClass()->IsChildOf(APlayerController::StaticClass()))
			{
				_ASSERT(!PlayerController); // PC가 하나만 있어야 합니다.
				PlayerController = Actor->As<APlayerController>().get();
			}
		}

		if (PlayerController)
		{
			PlayerController->UpdateCameraManager(DeltaSeconds);
		}
	}
}

TObjectPtr<UWorld> UWorld::GetDuplicatedWorldForPIE(UWorld* InWorld)
{
	TObjectPtr<UWorld> NewWorld = Cast<UWorld>(StaticDuplicateObject(InWorld, nullptr, TEXT("PIE World"), EDuplicateMode::PIE));
	NewWorld->WorldType = EWorldType::PIE;
	return NewWorld;
}

void UWorld::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	Ar << PersistentLevel;
}

string UWorld::Save()
{
	std::stringstream Buffer;
	boost::archive::text_oarchive SaveArchive = boost::archive::text_oarchive(Buffer);
	FArchive Ar = FArchive(SaveArchive);

	uint64 ObjectCount = PersistentLevel->Actors.size();
	Ar << ObjectCount;

	for (TEnginePtr<AActor> Actor : PersistentLevel->Actors)
	{
		Actor->Serialize(Ar);
	}

	string String = Buffer.str();
	return String;
}

void UWorld::Load(const string& InLoadString)
{
	std::stringstream Buffer = std::stringstream(InLoadString);
	boost::archive::text_iarchive LoadArchive = boost::archive::text_iarchive(Buffer);
	FArchive Ar = FArchive(LoadArchive);

	uint64 ObjectCount = 0;
	Ar << ObjectCount;

	for (uint64 i = 0; i < ObjectCount; ++i)
	{
		FString ObjectName;
		Ar << ObjectName;

		auto It = std::find_if(PersistentLevel->Actors.begin(), PersistentLevel->Actors.end(),
			[&ObjectName](TObjectPtr<AActor> Actor)
			{
				if (Actor->GetName() == ObjectName) { return true; }
				return false;
			}
		);

		if (It != PersistentLevel->Actors.end())
		{
			It->get()->Serialize(Ar);
		}
	}
}

AActor* UWorld::SpawnActor(UClass* Class, FTransform const* UserTransformPtr, const FActorSpawnParameters& SpawnParameters)
{
	_ASSERT(PersistentLevel.get());

	// Make sure this class is spawnable.
	if (!Class)
	{
		E_LOG(Warning, TEXT("SpawnActor failed because no class was specified"));
		return NULL;
	}
	else if (!Class->IsChildOf(AActor::StaticClass()))
	{
		E_LOG(Warning, TEXT("SpawnActor failed because {} is not an actor class"), Class->GetName());
		return NULL;
	}
	else if (SpawnParameters.Template != NULL && SpawnParameters.Template->GetClass() != Class)
	{
		E_LOG(Warning, TEXT("SpawnActor failed because template class ({}) does not match spawn class ({})"), SpawnParameters.Template->GetClass()->GetName(), Class->GetName());
		return NULL;
	}

	ULevel* LevelToSpawnIn = PersistentLevel.get();

	// 템플릿이 제공되지 않은 경우 클래스의 기본 액터를 템플릿으로 사용합니다.
	AActor* Template = SpawnParameters.Template ? SpawnParameters.Template : Class->GetDefaultObject<AActor>();
	_ASSERT(Template);

	FName NewActorName = SpawnParameters.Name;
	FTransform const UserTransform = UserTransformPtr ? *UserTransformPtr : FTransform::Identity;

	EObjectFlags ActorFlags = SpawnParameters.ObjectFlags;

	// actually make the actor object
	TObjectPtr<AActor> Actor = NewObject<AActor>(LevelToSpawnIn, Class, NewActorName, ActorFlags, Template);
	LevelToSpawnIn->Actors.push_back(Actor);

	AActor* NewActor = Actor.get();

	NewActor->PostSpawnInitialize(UserTransform, SpawnParameters.Owner, SpawnParameters.Instigator, SpawnParameters.TransformScaleMethod);

	// Broadcast notification of spawn
	OnActorSpawned.Broadcast(NewActor);
	E_LOG(Log, TEXT("Actor Spawned({})"), Class->GetName());
	E_LOG(Warning, TEXT("Actor Spawned({})"), Class->GetName());

	return NewActor;
}

APlayerController* UWorld::SpawnPlayActor(UPlayer* NewPlayer)
{
	if (AGameModeBase* const GameMode = GetAuthGameMode())
	{
		// Give the GameMode a chance to accept the login
		APlayerController* NewPlayerController = GameMode->Login(NewPlayer/*, RemoteRole, *InURL.Portal, Options, UniqueId, Error*/);
		if (NewPlayerController == NULL)
		{
			_ASSERT(false);
			//E_LOG(Warning, TEXT("Login failed: {}"), *Error);
			return NULL;
		}

		//E_LOG(Log, TEXT("{} got player {} [{}]"), NewPlayerController->GetName(), NewPlayer->GetName()/*, UniqueId.IsValid() ? *UniqueId->ToString() : TEXT("Invalid")*/);
		E_LOG(Log, TEXT("{} got player {}"), NewPlayerController->GetName(), NewPlayer->GetName());

		// 새로 스폰된 플레이어를 소유합니다.
		//NewPlayerController->NetPlayerIndex = InNetPlayerIndex;
		//NewPlayerController->SetRole(ROLE_Authority);
		//NewPlayerController->SetReplicates(RemoteRole != ROLE_None);
		//if (RemoteRole == ROLE_AutonomousProxy)
		//{
		//	NewPlayerController->SetAutonomousProxy(true);
		//}
		NewPlayerController->SetPlayer(NewPlayer);
		
		GameMode->PostLogin(NewPlayerController);

		return NewPlayerController;
	}

	E_LOG(Warning, TEXT("Login failed: No game mode set."));
	return nullptr;
}

bool UWorld::AreActorsInitialized() const
{
	return bActorsInitialized && PersistentLevel && PersistentLevel->Actors.size();
}

bool UWorld::IsGameWorld() const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE || WorldType == EWorldType::GamePreview || WorldType == EWorldType::GameRPC;
}

bool UWorld::IsEditorWorld() const
{
	return WorldType == EWorldType::Editor || WorldType == EWorldType::EditorPreview || WorldType == EWorldType::PIE;
}

void UWorld::OnWorldChanged()
{
	WorldChangedDelegate.Broadcast(this, PersistentLevel->Actors);
}

void UWorld::SetBegunPlay(bool bHasBegunPlay)
{
	if (bBegunPlay == bHasBegunPlay)
	{
		return;
	}

	bBegunPlay = bHasBegunPlay;
	/*if (OnBeginPlay.IsBound())
	{
		OnBeginPlay.Broadcast(bBegunPlay);
	}*/
}

bool UWorld::GetBegunPlay() const
{
	return bBegunPlay;
}

bool UWorld::HasBegunPlay() const
{
	return GetBegunPlay() && PersistentLevel && PersistentLevel->Actors.size();
}

bool UWorld::SetGameMode(const FURL& InURL)
{
	if (!AuthorityGameMode)
	{
		AuthorityGameMode = GetGameInstance()->CreateGameModeForURL(InURL, this);
		if (AuthorityGameMode != NULL)
		{
			return true;
		}
		else
		{
			E_LOG(Error, TEXT("Failed to spawn GameMode actor."));
			return false;
		}
	}
	return false;
}

void UWorld::BeginPlay()
{
//	const TArray<UWorldSubsystem*>& WorldSubsystems = SubsystemCollection.GetSubsystemArray<UWorldSubsystem>(UWorldSubsystem::StaticClass());
//
//	if (SupportsMakingVisibleTransactionRequests() && (IsNetMode(NM_DedicatedServer) || IsNetMode(NM_ListenServer)))
//	{
//		ServerStreamingLevelsVisibility = AServerStreamingLevelsVisibility::SpawnServerActor(this);
//	}
//
//#if WITH_EDITOR
//	// PIE(게임 내 에디터)/게임에서 사용되는 자산들이 완료될 수 있도록 기회를 줍니다.
//	FAssetCompilingManager::Get().ProcessAsyncTasks();
//#endif
//
//	for (UWorldSubsystem* WorldSubsystem : WorldSubsystems)
//	{
//		WorldSubsystem->OnWorldBeginPlay(*this);
//	}

	AGameModeBase* const GameMode = GetAuthGameMode();
	if (GameMode)
	{
		GameMode->StartPlay();
		/*if (GetAISystem())
		{
			GetAISystem()->StartPlay();
		}*/
	}

	/*OnWorldBeginPlay.Broadcast();

	if (PhysicsScene)
	{
		PhysicsScene->OnWorldBeginPlay();
	}*/

}
