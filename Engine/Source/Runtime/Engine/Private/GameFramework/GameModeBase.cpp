#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameSession.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/DefaultPawn.h"
#include "Engine/World.h"

AGameModeBase::AGameModeBase()
{
    PlayerControllerClass = APlayerController::StaticClass();
}

void AGameModeBase::PreInitializeComponents()
{
    Super::PreInitializeComponents();

    FActorSpawnParameters SpawnInfo;
    //SpawnInfo.Instigator = GetInstigator();
    SpawnInfo.ObjectFlags |= RF_Transient;  // GameState나 네트워크 관리자들을 맵에 저장하고 싶지 않습니다.

    // GameState가 지정되지 않은 경우 기본 GameState로 대체합니다.
    if (GameStateClass == nullptr)
    {
        E_LOG(Warning, TEXT("No GameStateClass was specified in {} ({})"), GetName(), GetClass()->GetName());
        GameStateClass = AGameStateBase::StaticClass();
    }

    UWorld* World = GetWorld();
    GameState = World->SpawnActor<AGameStateBase>(GameStateClass, SpawnInfo);
    World->SetGameState(GameState);
    if (GameState)
    {
        GameState->AuthorityGameMode = this;
    }

    // 네트워크 게임에서는 서버에만 NetworkManager가 필요합니다.
    /*   AWorldSettings* WorldSettings = World->GetWorldSettings();
    World->NetworkManager = WorldSettings->GameNetworkManagerClass ? World->SpawnActor<AGameNetworkManager>(WorldSettings->GameNetworkManagerClass, SpawnInfo) : nullptr;*/

    //InitGameState();
}

void AGameModeBase::InitGame()
{
    UWorld* World = GetWorld();

    FActorSpawnParameters SpawnInfo;
    //SpawnInfo.Instigator = GetInstigator();
    SpawnInfo.ObjectFlags |= RF_Transient;	// 게임 세션을 맵에 저장하지 않기를 원합니다.
    //GameSession = World->SpawnActor<AGameSession>(GetGameSessionClass(), SpawnInfo);
    GameSession = World->SpawnActor<AGameSession>(AGameSession::StaticClass(), SpawnInfo);
}

APlayerController* AGameModeBase::Login(UPlayer* NewPlayer)
{
    if (GameSession == nullptr)
    {
        //ErrorMessage = TEXT("Failed to spawn player controller, GameSession is null");
        _ASSERT(false);
        return nullptr;
    }

    // 접속자수 관리 등
    /* ErrorMessage = GameSession->ApproveLogin(Options);
    if (!ErrorMessage.IsEmpty())
    {
        return nullptr;
    }*/

    APlayerController* const NewPlayerController = SpawnPlayerController(/*InRemoteRole, Options*/);
    if (NewPlayerController == nullptr)
    {
        _ASSERT(false);
        // Handle spawn failure.
        /*UE_LOG(LogGameMode, Log, TEXT("Login: Couldn't spawn player controller of class %s"), PlayerControllerClass ? *PlayerControllerClass->GetName() : TEXT("NULL"));
        ErrorMessage = FString::Printf(TEXT("Failed to spawn player controller"));*/
        return nullptr;
    }

    return NewPlayerController;
}

void AGameModeBase::PostLogin(APlayerController* NewPlayer)
{
    // 서버 접속 중에도 발생할 수 있는 공통 초기화 실행
    //GenericPlayerInitialization(NewPlayer);

    // 서버에 처음 접속할 때만 발생하는 초기화 수행
    UWorld* World = GetWorld();

    //NewPlayer->ClientCapBandwidth(NewPlayer->Player->CurrentNetSpeed);

    //if (MustSpectate(NewPlayer))
    //{
    //    NewPlayer->ClientGotoState(NAME_Spectating);
    //}
    //else
    //{
    //    // NewPlayer가 단순한 관전자가 아니고 유효한 ID를 가진 경우, 이를 리플레이에 사용자로 추가합니다.
    //    const FUniqueNetIdRepl& NewPlayerStateUniqueId = NewPlayer->PlayerState->GetUniqueId();
    //    if (NewPlayerStateUniqueId.IsValid() && NewPlayerStateUniqueId.IsV1())
    //    {
    //        GetGameInstance()->AddUserToReplay(NewPlayerStateUniqueId.ToString());
    //    }
    //}

    if (GameSession)
    {
        GameSession->PostLogin(NewPlayer);
    }

    DispatchPostLogin(NewPlayer);

    // 초기화가 완료되었으므로, 플레이어의 폰을 생성하고 경기를 시작하려고 시도합니다.
    HandleStartingNewPlayer(NewPlayer);
}

void AGameModeBase::DispatchPostLogin(AController* NewPlayer)
{
    /*if (APlayerController* NewPC = Cast<APlayerController>(NewPlayer))
    {
        K2_PostLogin(NewPC);
        FGameModeEvents::GameModePostLoginEvent.Broadcast(this, NewPC);
    }*/

    OnPostLogin(NewPlayer);
}

void AGameModeBase::HandleStartingNewPlayer(APlayerController* NewPlayer)
{
    // 플레이어가 관전자로 시작해야 한다면, 관전자 상태로 둡니다
    //if (!bStartPlayersAsSpectators && !MustSpectate(NewPlayer) && PlayerCanRestart(NewPlayer))
    {
        // 그렇지 않으면 즉시 플레이어의 폰을 생성합니다
        RestartPlayer(NewPlayer);
    }
}

void AGameModeBase::RestartPlayer(AController* NewPlayer)
{
    if (NewPlayer == nullptr /*|| NewPlayer->IsPendingKillPending()*/)
    {
        return;
    }

    AActor* StartSpot = nullptr; // FindPlayerStart(NewPlayer);

    // 시작 위치를 찾지 못한 경우,
    if (StartSpot == nullptr)
    {
        E_LOG(Warning, TEXT("아직 StartSpot이 없습니다"));
        // 이전에 할당된 위치가 있는지 확인합니다.
        /*if (NewPlayer->StartSpot != nullptr)
        {
            StartSpot = NewPlayer->StartSpot.Get();
            UE_LOG(LogGameMode, Warning, TEXT("RestartPlayer: Player start not found, using last start spot"));
        }*/
    }

    RestartPlayerAtPlayerStart(NewPlayer, StartSpot);
}

void AGameModeBase::RestartPlayerAtPlayerStart(AController* NewPlayer, AActor* StartSpot)
{
    if (NewPlayer == nullptr/* || NewPlayer->IsPendingKillPending()*/)
    {
        return;
    }

    /*if (!StartSpot)
    {
        E_LOG(Warning, TEXT("RestartPlayerAtPlayerStart: Player start not found"));
        return;
    }*/

    FRotator SpawnRotation = FRotator::ZeroRotator;// StartSpot->GetActorRotation();

    //E_LOG(Log, TEXT("RestartPlayerAtPlayerStart {}"), (NewPlayer && NewPlayer->PlayerState) ? *NewPlayer->PlayerState->GetPlayerName() : TEXT("Unknown"));

    /*if (MustSpectate(Cast<APlayerController>(NewPlayer)))
    {
        UE_LOG(LogGameMode, Verbose, TEXT("RestartPlayerAtPlayerStart: Tried to restart a spectator-only player!"));
        return;
    }*/

    //if (NewPlayer->GetPawn() != nullptr)
    //{
    //    // 기존 폰이 있는 경우, 해당 폰의 회전을 사용합니다.
    //    SpawnRotation = NewPlayer->GetPawn()->GetActorRotation();
    //}
    //else if (GetDefaultPawnClassForController(NewPlayer) != nullptr)
    {
        // 이 플레이어를 위한 기본 클래스를 사용하여 폰을 생성하려고 시도합니다.
        //APawn* NewPawn = SpawnDefaultPawnFor(NewPlayer, StartSpot);
        UClass* DefaultPawn = DefaultPawnClass;
        if (!DefaultPawn)
        {
            DefaultPawn = ADefaultPawn::StaticClass();
        }
        APawn* NewPawn = GetWorld()->SpawnActor<APawn>(DefaultPawn);

        if (NewPawn)
        {
            NewPlayer->SetPawn(NewPawn);
        }
    }

    /*if (!IsValid(NewPlayer->GetPawn()))
    {
        FailedToRestartPlayer(NewPlayer);
    }
    else*/
    {
        // 시작 위치가 사용되었음을 알립니다.
        //InitStartSpot(StartSpot, NewPlayer);

        FinishRestartPlayer(NewPlayer, SpawnRotation);
    }

}

void AGameModeBase::FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation)
{
    NewPlayer->Possess(NewPlayer->GetPawn());

    // If the Pawn is destroyed as part of possession we have to abort
    if (!NewPlayer->GetPawn())
    {
        _ASSERT(false);
        //FailedToRestartPlayer(NewPlayer);
    }
    else
    {
        // Set initial control rotation to starting rotation rotation
        /*NewPlayer->ClientSetRotation(NewPlayer->GetPawn()->GetActorRotation(), true);

        FRotator NewControllerRot = StartRotation;
        NewControllerRot.Roll = 0.f;
        NewPlayer->SetControlRotation(NewControllerRot);

        SetPlayerDefaults(NewPlayer->GetPawn());

        K2_OnRestartPlayer(NewPlayer);*/
    }
}

APlayerController* AGameModeBase::SpawnPlayerController()
{
    FActorSpawnParameters SpawnInfo;
    //SpawnInfo.Instigator = GetInstigator();
    SpawnInfo.ObjectFlags |= RF_Transient;	// We never want to save player controllers into a map
    //SpawnInfo.bDeferConstruction = true;
    APlayerController* NewPC = GetWorld()->SpawnActor<APlayerController>(PlayerControllerClass, FTransform::Identity, SpawnInfo);
    //if (NewPC)
    //{
    //    if (InRemoteRole == ROLE_SimulatedProxy)
    //    {
    //        // 이는 권한/자율 원격 역할이 없기 때문에 로컬 플레이어입니다.
    //        NewPC->SetAsLocalPlayerController();
    //    }

    //    UGameplayStatics::FinishSpawningActor(NewPC, FTransform(SpawnRotation, SpawnLocation));
    //}

    return NewPC;
}

void AGameModeBase::StartPlay()
{
    GameState->HandleBeginPlay();
}
