#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"
#include "Components/InputComponent.h"

void APawn::GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const
{
	OutLocation = GetPawnViewLocation();
	OutRotation = GetViewRotation();
}

FVector APawn::GetPawnViewLocation() const
{
	return GetActorLocation() + FVector(0.f, 0.f, BaseEyeHeight);
}

FRotator APawn::GetViewRotation() const
{
	if (Controller != nullptr)
	{
		return Controller->GetControlRotation();
	}
	//else if (GetLocalRole() < ROLE_Authority)
	//{
	//	// check if being spectated
	//	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	//	{
	//		APlayerController* PlayerController = Iterator->Get();
	//		if (PlayerController &&
	//			PlayerController->PlayerCameraManager &&
	//			PlayerController->PlayerCameraManager->GetViewTargetPawn() == this)
	//		{
	//			return PlayerController->BlendedTargetViewRotation;
	//		}
	//	}
	//}

	return GetActorRotation();
}

AController* APawn::GetController() const
{
	return Controller;
}

void APawn::PossessedBy(AController* NewController)
{
	SetOwner(NewController);

	AController* const OldController = Controller;

	Controller = NewController->As<AController>();
	//ForceNetUpdate();

//#if UE_WITH_IRIS
//	// The owning connection depends on the Controller having the new value.
//	UpdateOwningNetConnection();
//#endif

	//if (Controller->PlayerState != nullptr)
	//{
	//	SetPlayerState(Controller->PlayerState);
	//}

	/*if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (GetNetMode() != NM_Standalone)
		{
			SetReplicates(true);
			SetAutonomousProxy(true);
		}
	}
	else
	{
		CopyRemoteRoleFrom(GetDefault<APawn>());
	}*/

	// dispatch Blueprint event if necessary
	/*if (OldController != NewController)
	{
		ReceivePossessed(Controller);

		NotifyControllerChanged();
	}*/
}

void APawn::DispatchRestart(bool bCallClientRestart)
{
	if (bCallClientRestart)
	{
		// This calls Restart()
		PawnClientRestart();
	}
	else
	{
		_ASSERT(false);
		//Restart();
	}

	//NotifyRestarted();
}

void APawn::PawnClientRestart()
{
	Restart();

	APlayerController* PC = Cast<APlayerController>(Controller);
	if (PC /*&& PC->IsLocalController()*/)
	{
		// Handle camera possession
		/*if (PC->bAutoManageActiveCameraTarget)
		{
			PC->AutoManageActiveCameraTarget(this);
		}*/

		// Set up player input component, if there isn't one already.
		if (InputComponent == nullptr)
		{
			InputComponent = CreatePlayerInputComponent();
			if (InputComponent)
			{
				SetupPlayerInputComponent(InputComponent);
				InputComponent->RegisterComponent();
				/*if (UInputDelegateBinding::SupportsInputDelegate(GetClass()))
				{
					InputComponent->bBlockInput = bBlockInput;
					UInputDelegateBinding::BindInputDelegatesWithSubojects(this, InputComponent);
				}*/
			}
		}
	}
}

void APawn::Restart()
{
	/*UPawnMovementComponent* MovementComponent = GetMovementComponent();
	if (MovementComponent)
	{
		MovementComponent->StopMovementImmediately();
	}
	ConsumeMovementInputVector();
	RecalculateBaseEyeHeight();*/
}

TEnginePtr<UInputComponent> APawn::CreatePlayerInputComponent()
{
	static const FName InputComponentName(TEXT("PawnInputComponent0"));
	//const UClass* OverrideClass = OverrideInputComponentClass.Get();
	return NewObject<UInputComponent>(this, nullptr, InputComponentName);
}
