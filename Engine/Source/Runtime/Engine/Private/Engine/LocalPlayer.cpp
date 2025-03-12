#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "SceneView.h"

void ULocalPlayer::PlayerAdded(UGameViewportClient* InViewportClient)
{
	ViewportClient = InViewportClient;
}

void ULocalPlayer::SpawnPlayActor(UWorld* InWorld)
{
	PlayerController = InWorld->SpawnPlayActor(this/*, ROLE_SimulatedProxy, PlayerURL, UniqueId, OutError, GEngine->GetGamePlayers(InWorld).Find(this)*/);
}

void ULocalPlayer::CalcSceneView(FSceneViewFamily* ViewFamily, FVector& OutViewLocation, FRotator& OutViewRotation)
{
	FMinimalViewInfo ViewInfo;
	GetViewPoint(ViewInfo);

	OutViewLocation = ViewInfo.Location;
	OutViewRotation = ViewInfo.Rotation;

	FRotator ViewRotation = OutViewRotation;

	ViewFamily->ViewRotationMatrix = FInverseRotationMatrix(ViewRotation);

	// 언리얼 축에 맞게 View Matrix를 회전하고 있습니다.
	// X가 전후방
	// Y가 좌우
	// Z가 상하
	ViewFamily->ViewRotationMatrix = ViewFamily->ViewRotationMatrix * FMatrix(
		FPlane(0, 0, 1, 0),
		FPlane(1, 0, 0, 0),
		FPlane(0, 1, 0, 0),
		FPlane(0, 0, 0, 1));

	FVector ViewOrigin = OutViewLocation;
	FMatrix ViewTranslationMatrix = FMatrix::CreateTranslation(-ViewOrigin);

	ViewFamily->ViewMatrix = ViewTranslationMatrix * ViewFamily->ViewRotationMatrix;
	ViewFamily->EyePosition = ViewOrigin;

	constexpr float FOV = 90.0;
	constexpr float RadianFOV = DirectX::XMConvertToRadians(FOV);
	const float HalfRadianFOV = RadianFOV / 2.f;

	float XAxisMultiplier;
	float YAxisMultiplier;
	const FVector2D ViewportSize = ViewFamily->ViewportSize;

	if (ViewportSize.x > ViewportSize.y)
	{
		XAxisMultiplier = 1.f;
		YAxisMultiplier = ViewportSize.x / (float)ViewportSize.y;
	}
	else
	{
		XAxisMultiplier = ViewportSize.y / (float)ViewportSize.x;
		YAxisMultiplier = 1.f;
	}

	ViewFamily->ProjectionMatrix = FReversedZPerspectiveMatrix(HalfRadianFOV, HalfRadianFOV, XAxisMultiplier, YAxisMultiplier, 10.f, 10.f);

	ViewFamily->ViewProjectionMatrix = ViewFamily->ViewMatrix * ViewFamily->ProjectionMatrix;
}

void ULocalPlayer::GetViewPoint(FMinimalViewInfo& OutViewInfo)
{
	_ASSERT(PlayerController);
	if (PlayerController != NULL)
	{
		_ASSERT(PlayerController->PlayerCameraManager);
		if (PlayerController->PlayerCameraManager != NULL)
		{
			OutViewInfo = PlayerController->PlayerCameraManager->GetCameraCacheView();
			//PlayerController->GetPlayerViewPoint(/*out*/ OutViewInfo.Location, /*out*/ OutViewInfo.Rotation);
		}
		else
		{
			//PlayerController->GetPlayerViewPoint(/*out*/ OutViewInfo.Location, /*out*/ OutViewInfo.Rotation);
		}
	}
}
