#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"

FRotator AController::GetControlRotation() const
{
    return ControlRotation;
}

void AController::SetPawn(APawn* InPawn)
{
	//RemovePawnTickDependency(Pawn);

	Pawn = InPawn->As<APawn>();
	//Character = (Pawn ? Cast<ACharacter>(Pawn) : NULL);

	//AttachToPawn(Pawn);

	//AddPawnTickDependency(Pawn);
}

void AController::Possess(APawn* InPawn)
{
    /*if (!bCanPossessWithoutAuthority && !HasAuthority())
        {
            FMessageLog("PIE").Warning(FText::Format(
                LOCTEXT("ControllerPossessAuthorityOnly", "Possess 함수는 네트워크 권한이 있는 경우에만 사용해야 합니다: {0}"),
                FText::FromName(GetFName())
            ));
            UE_LOG(LogController, Warning, TEXT("네트워크 권한 없이 %s를 소유하려고 합니다! 요청이 무시됩니다."), *GetNameSafe(InPawn));
            return;
        }

        REDIRECT_OBJECT_TO_VLOG(InPawn, this);*/

    APawn* CurrentPawn = GetPawn();

    // 현재 할당된 폰이 소유되지 않은 경우 알림이 필요합니다 (즉, Possess 호출 전에 할당된 폰).
    const bool bNotificationRequired = (CurrentPawn != nullptr) && (CurrentPawn->GetController() == nullptr);

    // 이전 호환성을 유지하기 위해 null 폰에 대해 알림을 계속 보냅니다.
    // 기본 엔진 구현은 현재 폰의 소유를 해제하는 것입니다.
    OnPossess(InPawn);

    // 네이티브 클래스에 의해 허용된 폰이 할당된 폰과 다르거나 알림이 명시적으로 필요한 경우 알림을 보냅니다.
    /*APawn* NewPawn = GetPawn();
    if ((NewPawn != CurrentPawn) || bNotificationRequired)
    {
        ReceivePossess(NewPawn);
        OnNewPawn.Broadcast(NewPawn);
        OnPossessedPawnChanged.Broadcast(bNotificationRequired ? nullptr : CurrentPawn, NewPawn);
    }

    TRACE_PAWN_POSSESS(this, InPawn);*/

}

void AController::OnPossess(APawn* InPawn)
{
    _ASSERT(false); // 아직 미구현
}
