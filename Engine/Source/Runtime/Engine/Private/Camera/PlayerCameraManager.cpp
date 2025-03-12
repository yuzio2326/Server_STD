#include "Camera/PlayerCameraManager.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

void APlayerCameraManager::LimitViewPitch(FRotator& ViewRotation, float InViewPitchMin, float InViewPitchMax)
{
	ViewRotation.Pitch = FMath::ClampAngle(ViewRotation.Pitch, InViewPitchMin, InViewPitchMax);
	ViewRotation.Pitch = FRotator::ClampAxis(ViewRotation.Pitch);
}

void APlayerCameraManager::LimitViewRoll(FRotator& ViewRotation, float InViewRollMin, float InViewRollMax)
{
	ViewRotation.Roll = FMath::ClampAngle(ViewRotation.Roll, InViewRollMin, InViewRollMax);
	ViewRotation.Roll = FRotator::ClampAxis(ViewRotation.Roll);
}

void APlayerCameraManager::LimitViewYaw(FRotator& ViewRotation, float InViewYawMin, float InViewYawMax)
{
	ViewRotation.Yaw = FMath::ClampAngle(ViewRotation.Yaw, InViewYawMin, InViewYawMax);
	ViewRotation.Yaw = FRotator::ClampAxis(ViewRotation.Yaw);
}

void APlayerCameraManager::InitializeFor(APlayerController* PC)
{
	// FMinimalViewInfo DefaultFOVCache = GetCameraCacheView();
	// DefaultFOVCache.FOV = DefaultFOV;
	// SetCameraCachePOV(DefaultFOVCache);

	PCOwner = PC;

	//SetViewTarget(PC);

	//// 레벨 기본 스케일을 설정합니다.
	//SetDesiredColorScale(GetWorldSettings()->DefaultColorScale, 5.f);

	//// 카메라가 전체 틱 동안 (0,0,0)에 위치하지 않도록 카메라 업데이트를 강제합니다.
	//// 이것은 스트리밍에 부작용을 일으킬 수 있습니다.
	//UpdateCamera(0.f);

}

void APlayerCameraManager::ProcessViewRotation(float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot)
{
	//SCOPE_CYCLE_COUNTER(STAT_Camera_ProcessViewRotation);
	//for (int32 ModifierIdx = 0; ModifierIdx < ModifierList.Num(); ModifierIdx++)
	//{
	//	if (ModifierList[ModifierIdx] != NULL &&
	//		!ModifierList[ModifierIdx]->IsDisabled())
	//	{
	//		if (ModifierList[ModifierIdx]->ProcessViewRotation(ViewTarget.Target, DeltaTime, OutViewRotation, OutDeltaRot))
	//		{
	//			break;
	//		}
	//	}
	//}

	// Add Delta Rotation
	OutViewRotation += OutDeltaRot;
	OutDeltaRot = FRotator::ZeroRotator;

	//const bool bIsHeadTrackingAllowed =
	//	GEngine->XRSystem.IsValid() &&
	//	(GetWorld() != nullptr ? GEngine->XRSystem->IsHeadTrackingAllowedForWorld(*GetWorld()) : GEngine->XRSystem->IsHeadTrackingAllowed());
	//if (bIsHeadTrackingAllowed)
	//{
	//	// With the HMD devices, we can't limit the view pitch, because it's bound to the player's head.  A simple normalization will suffice
	//	OutViewRotation.Normalize();
	//}
	//else
	{
		// Limit Player View Axes
		LimitViewPitch(OutViewRotation, ViewPitchMin, ViewPitchMax);
		LimitViewYaw(OutViewRotation, ViewYawMin, ViewYawMax);
		LimitViewRoll(OutViewRotation, ViewRollMin, ViewRollMax);
	}
}

void APlayerCameraManager::UpdateCamera(float DeltaTime)
{
	APawn* Pawn = PCOwner->GetPawn();
	_ASSERT(Pawn);
	if (Pawn)
	{
		Pawn->CalcCamera(DeltaTime, CameraCachePOV);
	}
}

const FMinimalViewInfo& APlayerCameraManager::GetCameraCacheView() const
{
	return CameraCachePOV;
}
