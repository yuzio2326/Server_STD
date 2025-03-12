#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerInput.h"
#include "Components/InputComponent.h"
#include "Engine/LocalPlayer.h"
#include "Engine/Player.h"
#include "Engine/World.h"

void APlayerController::AddYawInput(float Val)
{
	RotationInput.Yaw += Val;
}

void APlayerController::AddPitchInput(float Val)
{
	RotationInput.Pitch += Val;
}

void APlayerController::UpdateRotation(float DeltaTime)
{
	// Calculate Delta to be applied on ViewRotation
	FRotator DeltaRot(RotationInput);

	FRotator ViewRotation = GetControlRotation();

	if (PlayerCameraManager)
	{
		PlayerCameraManager->ProcessViewRotation(DeltaTime, ViewRotation, DeltaRot);
	}

	/*AActor* ViewTarget = GetViewTarget();
	if (!PlayerCameraManager || !ViewTarget || !ViewTarget->HasActiveCameraComponent() || ViewTarget->HasActivePawnControlCameraComponent())
	{
		if (IsLocalPlayerController() && GEngine->XRSystem.IsValid() && GetWorld() != nullptr && GEngine->XRSystem->IsHeadTrackingAllowedForWorld(*GetWorld()))
		{
			auto XRCamera = GEngine->XRSystem->GetXRCamera();
			if (XRCamera.IsValid())
			{
				XRCamera->ApplyHMDRotation(this, ViewRotation);
			}
		}
	}*/

	SetControlRotation(ViewRotation);
	/*
	APawn* const P = GetPawnOrSpectator();
	if (P)
	{
		P->FaceRotation(ViewRotation, DeltaTime);
	}*/
}

void APlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	InitPlayerState();

	SpawnPlayerCameraManager();
	// ResetCameraMode();

	//if (GetNetMode() == NM_Client)
	//{
	//	SpawnDefaultHUD();
	//}

	//AddCheats();

	//bPlayerIsWaiting = true;
	//StateName = NAME_Spectating; // ChangeState를 사용하지 마십시오. 플레이어가 수신될 때까지 SpectatorPawn을 생성하는 것을 지연시키기 원합니다.
}

void APlayerController::SpawnPlayerCameraManager()
{
	// 서버와 소유한 클라이언트는 카메라를 받습니다.
	// 아키타입이 지정되지 않은 경우, Engine.PlayerCameraManager를 생성합니다. 모든 게임은 아키타입을 지정해야 합니다.
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Owner = this;
	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.ObjectFlags |= RF_Transient;  // 카메라 매니저를 맵에 저장하지 않기를 원합니다.

	if (PlayerCameraManagerClass != NULL)
	{
		PlayerCameraManager = GetWorld()->SpawnActor<APlayerCameraManager>(PlayerCameraManagerClass, SpawnInfo);
	}
	else
	{
		PlayerCameraManager = GetWorld()->SpawnActor<APlayerCameraManager>(nullptr, SpawnInfo);
	}

	if (PlayerCameraManager != NULL)
	{
		PlayerCameraManager->InitializeFor(this);
	}
	else
	{
		E_LOG(Log, TEXT("Couldn't Spawn PlayerCameraManager for Player!!"));
	}
}

void APlayerController::UpdateCameraManager(float DeltaSeconds)
{
	if (PlayerCameraManager != NULL)
	{
		PlayerCameraManager->UpdateCamera(DeltaSeconds);
	}
}

void APlayerController::InitPlayerState()
{
	//if (GetNetMode() != NM_Client)
	{
		UWorld* const World = GetWorld();
		const AGameModeBase* GameMode = World ? World->GetAuthGameMode() : NULL;

		// GameMode가 null인 경우, 이는 리플레이를 기록하려고 하는 네트워크 클라이언트일 수 있습니다.
		// 이 경우에도 PlayerState를 생성할 수 있도록 기본 게임 모드를 사용해 보십시오.
		/*if (GameMode == NULL)
		{
			const AGameStateBase* const GameState = World ? World->GetGameState() : NULL;
			GameMode = GameState ? GameState->GetDefaultGameMode() : NULL;
		}*/

		if (GameMode != NULL)
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.Owner = this;
			SpawnInfo.Instigator = GetInstigator();
			//SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnInfo.ObjectFlags |= RF_Transient;  // 플레이어 상태를 맵에 저장하지 않으려면

			TSubclassOf<APlayerState> PlayerStateClassToSpawn = GameMode->PlayerStateClass;
			if (PlayerStateClassToSpawn.Get() == nullptr)
			{
				E_LOG(Log, TEXT("AController::InitPlayerState: 게임 모드 {}의 PlayerStateClass가 null입니다. APlayerState로 대체합니다."), 
					GameMode->GetName());
				PlayerStateClassToSpawn = APlayerState::StaticClass();
			}

			PlayerState = World->SpawnActor<APlayerState>(PlayerStateClassToSpawn, SpawnInfo)->As<APlayerState>();

			// 필요한 경우 기본 플레이어 이름을 강제로 설정합니다.
			if (PlayerState && PlayerState->GetPlayerName().empty())
			{
				// SetPlayerName()을 호출하지 않습니다. 이는 엔트리 메시지를 브로드캐스트하지만 GameMode가 아직
				// 플레이어/봇 이름을 적용할 기회를 가지지 않았기 때문입니다.

				//PlayerState->SetPlayerNameInternal(GameMode->DefaultPlayerName.ToString());
				PlayerState->SetPlayerName(TEXT("Hello World!")); // 원래는 비어 있음
			}
		}
	}

}

void APlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	TickPlayerInput(DeltaSeconds, DeltaSeconds == 0.f);

	UpdateRotation(DeltaSeconds);

	RotationInput = FRotator::ZeroRotator;
}

void APlayerController::TickPlayerInput(const float DeltaSeconds, const bool bGamePaused)
{
	// PlayerInput->Tick();

	//if (ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player))
	//{
	//	if (bEnableMouseOverEvents)
	//	{
	//		FVector2D MousePosition;
	//		FHitResult HitResult;
	//		bool bHit = false;

	//		UGameViewportClient* ViewportClient = LocalPlayer->ViewportClient;

	//		// Only send mouse hit events if we're directly over the viewport.
	//		if (IsInViewportClient(ViewportClient))
	//		{
	//			if (ViewportClient->GetMousePosition(MousePosition))
	//			{
	//				bHit = GetHitResultAtScreenPosition(MousePosition, CurrentClickTraceChannel, true, /*out*/ HitResult);
	//			}
	//		}

	//		UPrimitiveComponent* PreviousComponent = CurrentClickablePrimitive.Get();
	//		UPrimitiveComponent* CurrentComponent = (bHit ? HitResult.Component.Get() : NULL);

	//		UPrimitiveComponent::DispatchMouseOverEvents(PreviousComponent, CurrentComponent);

	//		CurrentClickablePrimitive = CurrentComponent;
	//	}

	//	if (bEnableTouchOverEvents)
	//	{
	//		for (int32 TouchIndexInt = 0; TouchIndexInt < EKeys::NUM_TOUCH_KEYS; ++TouchIndexInt)
	//		{
	//			const ETouchIndex::Type FingerIndex = ETouchIndex::Type(TouchIndexInt);

	//			FHitResult HitResult;
	//			const bool bHit = GetHitResultUnderFinger(FingerIndex, CurrentClickTraceChannel, true, /*out*/ HitResult);

	//			UPrimitiveComponent* PreviousComponent = CurrentTouchablePrimitives[TouchIndexInt].Get();
	//			UPrimitiveComponent* CurrentComponent = (bHit ? HitResult.Component.Get() : NULL);

	//			UPrimitiveComponent::DispatchTouchOverEvents(FingerIndex, PreviousComponent, CurrentComponent);

	//			CurrentTouchablePrimitives[TouchIndexInt] = CurrentComponent;
	//		}
	//	}
	//}

	ProcessPlayerInput(DeltaSeconds, bGamePaused);
	//ProcessForceFeedbackAndHaptics(DeltaSeconds, bGamePaused);
}

void APlayerController::ProcessPlayerInput(const float DeltaTime, const bool bGamePaused)
{
	TArray<UInputComponent*> InputStack;

	// 스택에 있는 모든 입력 컴포넌트를 상단부터 처리합니다.
	{
		//SCOPE_CYCLE_COUNTER(STAT_PC_BuildInputStack);
		BuildInputStack(InputStack);
	}

	// 원하는 컴포넌트를 처리합니다
	{
		//SCOPE_CYCLE_COUNTER(STAT_PC_ProcessInputStack);
		PlayerInput->ProcessInputStack(InputStack, DeltaTime, bGamePaused);
	}
}

void APlayerController::BuildInputStack(TArray<UInputComponent*>& InputStack)
{
	// 제어되는 폰(pawn)은 입력 스택에서 마지막으로 처리됩니다
	//APawn* ControlledPawn = GetPawnOrSpectator();
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn)
	{
		//if (ControlledPawn->InputEnabled())
		{
			// 폰 소유 시 생성된 명시적 입력 컴포넌트를 가져옵니다. 이 컴포넌트는 마지막으로 처리됩니다.
			if (ControlledPawn->InputComponent)
			{
				InputStack.push_back(ControlledPawn->InputComponent);
			}

			// 폰의 컴포넌트 배열에 추가된 다른 InputComponent가 있는지 확인합니다(스크립트에 의해 추가되었을 수 있습니다).
			for (TEnginePtr<UActorComponent> ActorComponent : ControlledPawn->GetComponents())
			{
				UInputComponent* PawnInputComponent = Cast<UInputComponent>(ActorComponent);
				if (PawnInputComponent && PawnInputComponent != ControlledPawn->InputComponent)
				{
					InputStack.push_back(PawnInputComponent);
				}
			}
		}
	}

	// LevelScriptActors는 스택에 다음으로 추가됩니다
	//for (ULevel* Level : GetWorld()->GetLevels())
	//{
	//	ALevelScriptActor* ScriptActor = Level->GetLevelScriptActor();
	//	if (ScriptActor)
	//	{
	//		if (ScriptActor->InputEnabled() && ScriptActor->InputComponent)
	//		{
	//			InputStack.Push(ScriptActor->InputComponent);
	//		}
	//	}
	//}

	//if (InputEnabled())
	{
		InputStack.push_back(InputComponent.get());
	}

	// 스택에 푸시된 컴포넌트가 우선 순위를 가집니다
	/*for (int32 Idx = 0; Idx < CurrentInputStack.Num(); ++Idx)
	{
		UInputComponent* IC = CurrentInputStack[Idx].Get();
		if (IC)
		{
			InputStack.Push(IC);
		}
		else
		{
			CurrentInputStack.RemoveAt(Idx--);
		}
	}*/
}

void APlayerController::SetPlayer(UPlayer* InPlayer)
{
	// Set the viewport.
	Player = InPlayer;
	InPlayer->PlayerController = this;

	// 로컬 플레이어에만 해당되는 초기화
	ULocalPlayer* LP = dynamic_cast<ULocalPlayer*>(InPlayer);
	if (LP != NULL)
	{
		// 클라이언트는 이것을 로컬로 표시해야 합니다 (서버는 이미 생성 시 알았습니다)
		// SetAsLocalPlayerController();
		//LP->InitOnlineSession();
		InitInputSystem();
	}
	else
	{
		_ASSERT(false);
	}
}

void APlayerController::InitInputSystem()
{
	if (PlayerInput == nullptr)
	{
		//const UClass* OverrideClass = OverridePlayerInputClass.Get();

		//PlayerInput = NewObject<UPlayerInput>(this, OverrideClass ? OverrideClass : UInputSettings::GetDefaultPlayerInputClass());
		PlayerInput = NewObject<UPlayerInput>(this, UPlayerInput::StaticClass());
	}

	SetupInputComponent();
}

void APlayerController::SetControlRotation(const FRotator& NewRotation)
{
	/*if (!IsValidControlRotation(NewRotation))
	{
		logOrEnsureNanError(TEXT("AController::SetControlRotation attempted to apply NaN-containing or NaN-causing rotation! (%s)"), *NewRotation.ToString());
		return;
	}*/

	if (!ControlRotation.Equals(NewRotation, 1e-3f))
	{
		ControlRotation = NewRotation;

		/*if (RootComponent && RootComponent->IsUsingAbsoluteRotation())
		{
			RootComponent->SetWorldRotation(GetControlRotation());
		}*/
	}
	else
	{
		//UE_LOG(LogPlayerController, Log, TEXT("Skipping SetControlRotation %s for %s (Pawn %s)"), *NewRotation.ToString(), *GetNameSafe(this), *GetNameSafe(GetPawn()));
	}
}

void APlayerController::SetupInputComponent()
{
	// 서브클래스는 다른 InputComponent 클래스를 생성할 수 있지만 기본 바인딩을 여전히 원할 수 있습니다
	if (InputComponent == NULL)
	{
		//InputComponent = NewObject<UInputComponent>(this, UInputSettings::GetDefaultInputComponentClass(), TEXT("PC_InputComponent0"));
		InputComponent = NewObject<UInputComponent>(this, UInputComponent::StaticClass(), TEXT("PC_InputComponent0"));
		InputComponent->RegisterComponent();
	}

	/*if (UInputDelegateBinding::SupportsInputDelegate(GetClass()))
	{
		InputComponent->bBlockInput = bBlockInput;
		UInputDelegateBinding::BindInputDelegatesWithSubojects(this, InputComponent);
	}*/
}

void APlayerController::OnPossess(APawn* PawnToPossess)
{
	if (PawnToPossess != NULL/* &&
		(PlayerState == NULL || !PlayerState->IsOnlyASpectator())*/)
	{
		const bool bNewPawn = (GetPawn() != PawnToPossess);

		if (GetPawn() && bNewPawn)
		{
			_ASSERT(false);
			//UnPossess();
		}

		if (PawnToPossess->Controller != NULL)
		{
			_ASSERT(false);
			//PawnToPossess->Controller->UnPossess();
		}

		PawnToPossess->PossessedBy(this);

		// 조종 중인 폰의 회전에 맞게 회전을 업데이트합니다.
		SetControlRotation(PawnToPossess->GetActorRotation());

		SetPawn(PawnToPossess);
		_ASSERT(GetPawn() != NULL);

		/*if (GetPawn() && GetPawn()->PrimaryActorTick.bStartWithTickEnabled)
		{
			GetPawn()->SetActorTickEnabled(true);
		}*/

		/*INetworkPredictionInterface* NetworkPredictionInterface = GetPawn() ? Cast<INetworkPredictionInterface>(GetPawn()->GetMovementComponent()) : NULL;
		if (NetworkPredictionInterface)
		{
			NetworkPredictionInterface->ResetPredictionData_Server();
		}*/

		//AcknowledgedPawn = NULL;

		// 로컬 PC는 ClientRestart에서 바로 Restart()가 트리거됩니다 (PawnClientRestart()를 통해), 하지만 서버는 원격 PC에 대해 로컬로 Restart()를 호출해야 합니다.
		// 우리는 실제로 Restart()를 여러 번 호출하지 않기 위해 노력하고 있습니다.
		/*if (!IsLocalPlayerController())
		{
			GetPawn()->DispatchRestart(false);
		}*/

		ClientRestart(GetPawn());

		/*ChangeState(NAME_Playing);
		if (bAutoManageActiveCameraTarget)
		{
			AutoManageActiveCameraTarget(GetPawn());
			ResetCameraMode();
		}*/
	}
}

void APlayerController::ClientRestart(APawn* NewPawn)
{
	//SetPawn(NewPawn);
	//GetPawn()->Controller = this;
	GetPawn()->DispatchRestart(true);
}
