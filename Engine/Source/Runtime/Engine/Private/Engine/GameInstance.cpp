#include "Engine/GameInstance.h"
#include "Engine/Engine.h"
#include "Engine/LocalPlayer.h"
#include "GameMapsSettings.h"
#include "GameFramework/GameModeBase.h"

UGameInstance::UGameInstance()
{
}

UEngine* UGameInstance::GetEngine() const
{
	return CastChecked<UEngine>(GetOuter()).get();
}

UGameViewportClient* UGameInstance::GetGameViewportClient() const
{
	return GetWorld()->GameViewport;
}

ULocalPlayer* UGameInstance::GetLocalPlayer(const uint64 PlayerIndex)
{
	if (LocalPlayers.size() < PlayerIndex + 1)
	{
		_ASSERT(false);
		return nullptr;
	}

	if (PlayerIndex != 0)
	{
		// 우리는 하나만 있음
		_ASSERT(false);
		return nullptr;
	}

	return LocalPlayers[PlayerIndex].get();
}

//#if WITH_EDITOR
void UGameInstance::InitializeForPlayInEditor()
{
	UEngine* const EditorEngine = GetEngine();
	UWorld* EditorWorld = EditorEngine->GetEditorWorld();

	// 표준 PIE path: 에디터 월드를 단순히 복제합니다.
	TObjectPtr<UWorld> PIEWorld = EditorEngine->CreatePIEWorldByDuplication(EditorWorld);

	EditorEngine->PlayWorld = PIEWorld;
	PIEWorld->SetGameInstance(this);
	World = PIEWorld.get();

	// World 컨텍스트를 설정하고 게임 인스턴스를 초기화한 후 월드를 초기화하여 일반적인 로드와 일관되게 합니다.
	// 이 작업은 월드 서브시스템을 생성하고 게임 시작을 준비합니다.
	EditorEngine->PostCreatePIEWorld(PIEWorld.get());
}

void UGameInstance::StartPlayInEditorGameInstance(ULocalPlayer* LocalPlayer)
{
	// 현재 월드에서 게임 플레이를 할 예정이므로, 플레이 준비를 합니다
	UWorld* const PlayWorld = GetWorld();

	// 생략
	//SpawnPlayFromHereStart
	{

	}

	// GameMode 생성 및 세팅
	{
		FURL URL = {};
		PlayWorld->SetGameMode(URL);
	}

	// InitializeActorsForPlay
	{
		FRegisterComponentContext Context(PlayWorld);
		PlayWorld->InitializeActorsForPlay(&Context);
		Context.Process();
	}

	if (LocalPlayer)
	{
		LocalPlayer->SpawnPlayActor(/*URL.ToString(1), Error,*/ PlayWorld);
		//if (!LocalPlayer->SpawnPlayActor(/*URL.ToString(1), Error,*/ PlayWorld))
		//{
		//	_ASSERT(false);
		//	return;
		//	//return FGameInstancePIEResult::Failure(FText::Format(NSLOCTEXT("UnrealEd", "Error_CouldntSpawnPlayer", "Couldn't spawn player: {0}"), FText::FromString(Error)));
		//}
	}

	PlayWorld->BeginPlay();
}

//#endif

ULocalPlayer* UGameInstance::CreateInitialPlayer()
{
	return CreateLocalPlayer(false);
}

ULocalPlayer* UGameInstance::CreateLocalPlayer(bool bSpawnPlayerController)
{
	TObjectPtr<ULocalPlayer> NewPlayer;
	NewPlayer = NewObject<ULocalPlayer>(GetEngine(), ULocalPlayer::StaticClass());// GetEngine()->LocalPlayerClass);
	AddLocalPlayer(NewPlayer);

	return NewPlayer.get();
}

int32 UGameInstance::AddLocalPlayer(TObjectPtr<ULocalPlayer> NewLocalPlayer)
{
	if (NewLocalPlayer == nullptr)
	{
		_ASSERT(false);
		return INDEX_NONE;
	}
	// Add to list
	LocalPlayers.push_back(NewLocalPlayer);
	const int32 InsertIndex = LocalPlayers.size() - 1;
	// Notify the player they were added
	NewLocalPlayer->PlayerAdded(GetGameViewportClient()/*, UserId*/);

	return InsertIndex;
}

AGameModeBase* UGameInstance::CreateGameModeForURL(FURL InURL, UWorld* InWorld)
{
	// 게임 모드 클래스를 가져옵니다. 처음에는 맵의 worldsettings에 지정된 기본 게임 유형을 사용합니다. 아래 설정으로 재정의될 수 있습니다.
	//TSubclassOf<AGameModeBase> GameClass = Settings->DefaultGameMode;

	const UGameMapsSettings* GameMapsSettings = GetDefault<UGameMapsSettings>();
	TSubclassOf<AGameModeBase> GameClass = GameMapsSettings->GameModeClass;
	if (!GameClass)
	{
		GameClass = AGameModeBase::StaticClass();
	}

	// Spawn the GameMode.
	E_LOG(Log, TEXT("Game class is '{}'"), GameClass->GetName());
	FActorSpawnParameters SpawnInfo;
	//SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnInfo.ObjectFlags |= RF_Transient;	// 저는 맵에 게임 모드를 저장하지 않는 것을 원합니다.

	return World->SpawnActor<AGameModeBase>(GameClass, SpawnInfo);
}
