#include "Editor/EditorViewportClient.h"
#include "Editor/CameraController.h"
#include "RenderResource.h"
#include "RendererInterface.h"
#include "SceneView.h"
#include "EngineModule.h"
#include "Engine/World.h"

UEditorViewportClient::UEditorViewportClient()
{
	if (HasAnyFlags(EObjectFlags::RF_ClassDefaultObject)) { return; }

	CameraUserImpulseData = new FCameraControllerUserImpulseData();
	CameraController = new FEditorCameraController();
	LastMouseState = DirectX::Mouse::Get().GetState();
}

UEditorViewportClient::~UEditorViewportClient()
{
	if (CameraUserImpulseData)
	{
		delete CameraUserImpulseData;
		CameraUserImpulseData = nullptr;
	}

	if (CameraController)
	{
		delete CameraController;
		CameraController = nullptr;
	}
}

void UEditorViewportClient::Init(HWND hInViewportHandle, UWorld* InWorld)
{
	Super::Init(hInViewportHandle, InWorld);
}

void UEditorViewportClient::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateCameraMovement(DeltaTime);
	UpdateMouseDelta();
}

void UEditorViewportClient::Draw()
{
	//Super::Draw();

	if (!GetScene()) { return; }
	// 뷰포트를 위한 FSceneViewFamily/FSceneView 설정
	FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(Viewport, GetScene()));
	ViewFamily.ViewportSize = ViewportSize;
	CalcSceneView(&ViewFamily);
	GetRendererModule().BeginRenderingViewFamily(&ViewFamily);
}

void UEditorViewportClient::UpdateCameraMovement(float DeltaTime)
{
	DirectX::Keyboard::State KeyboardState = DirectX::Keyboard::Get().GetState();

	// 확대 제스처에서 임펄스를 적용하고, WASD 키를 사용하는 경우 임펄스를 초기화합니다.
	CameraUserImpulseData->MoveForwardBackwardImpulse = 0.0f;// GestureMoveForwardBackwardImpulse;
	CameraUserImpulseData->MoveRightLeftImpulse = 0.0f;
	CameraUserImpulseData->MoveUpDownImpulse = 0.0f;
	CameraUserImpulseData->RotateYawImpulse = 0.0f;
	CameraUserImpulseData->RotatePitchImpulse = 0.0f;
	CameraUserImpulseData->RotateRollImpulse = 0.0f;

	DirectX::Mouse::State CurrentMouseState = DirectX::Mouse::Get().GetState();
	bool bRightBtnKeyState = CurrentMouseState.rightButton;
	bool bForwardKeyState = KeyboardState.IsKeyDown(DirectX::Keyboard::W);
	bool bBackwardKeyState = KeyboardState.IsKeyDown(DirectX::Keyboard::S);
	bool bRightKeyState = KeyboardState.IsKeyDown(DirectX::Keyboard::D);
	bool bLeftKeyState = KeyboardState.IsKeyDown(DirectX::Keyboard::A);
	bool bUpKeyState = KeyboardState.IsKeyDown(DirectX::Keyboard::E);
	bool bDownKeyState = KeyboardState.IsKeyDown(DirectX::Keyboard::Q);

	if (bRightBtnKeyState)
	{
		if (bForwardKeyState)
		{
			CameraUserImpulseData->MoveForwardBackwardImpulse += 1.f;
		}

		if (bBackwardKeyState)
		{
			CameraUserImpulseData->MoveForwardBackwardImpulse -= 1.f;
		}

		if (bRightKeyState)
		{
			CameraUserImpulseData->MoveRightLeftImpulse += 1.f;
		}

		if (bLeftKeyState)
		{
			CameraUserImpulseData->MoveRightLeftImpulse -= 1.f;
		}

		if (bUpKeyState)
		{
			CameraUserImpulseData->MoveUpDownImpulse += 1.f;
		}

		if (bDownKeyState)
		{
			CameraUserImpulseData->MoveUpDownImpulse -= 1.f;
		}
	}

	FVector NewViewLocation = GetViewLocation();
	FRotator NewViewRotation = GetViewRotation();

	// 카메라가 마우스나 키보드로 이동 중인지 확인합니다.
	//bool bHasMovement = false;// GetDefault<ULevelEditorViewportSettings>()->bUseLegacyCameraMovementNotifications;
	const FCameraControllerUserImpulseData& ImpulseData = *CameraUserImpulseData;

	/*if (ImpulseData.RotateYawVelocityModifier != 0.0f ||
		ImpulseData.RotatePitchVelocityModifier != 0.0f ||
		ImpulseData.RotateRollVelocityModifier != 0.0f ||
		ImpulseData.MoveForwardBackwardImpulse != 0.0f ||
		ImpulseData.MoveRightLeftImpulse != 0.0f ||
		ImpulseData.MoveUpDownImpulse != 0.0f ||
		ImpulseData.RotateYawImpulse != 0.0f ||
		ImpulseData.RotatePitchImpulse != 0.0f ||
		ImpulseData.RotateRollImpulse != 0.0f)
	{
		bHasMovement = true;
	}*/

	const float FinalCameraSpeedScale = 10000.f;

	CameraController->UpdateSimulation(
		*CameraUserImpulseData,
		DeltaTime,
		FinalCameraSpeedScale,
		NewViewLocation, NewViewRotation);

	// 시뮬레이션을 업데이트한 후 회전 속도 수정자를 0으로 초기화합니다. 
	// 이러한 동작은 항상 순간적이기 때문입니다. 즉, 사용자가 마우스 움직임으로 
	// 몇 픽셀을 이동할 때 즉시 임펄스 값을 증가시킵니다.
	{
		CameraUserImpulseData->RotateYawVelocityModifier = 0.0f;
		CameraUserImpulseData->RotatePitchVelocityModifier = 0.0f;
		CameraUserImpulseData->RotateRollVelocityModifier = 0.0f;
	}

	// 위치/회전이 변경되었는지 확인
	const bool bTransformDifferent = !NewViewLocation.Equals(GetViewLocation(), UE_SMALL_NUMBER) || NewViewRotation != GetViewRotation();
	if (bTransformDifferent)
	{
		SetViewLocation(NewViewLocation);
	}
}

void UEditorViewportClient::UpdateMouseDelta()
{
	struct FUpdateLastMouseState
	{
		~FUpdateLastMouseState()
		{
			LastMouseState = CurrentMouseState;
		}
		DirectX::Mouse::State& LastMouseState;
		DirectX::Mouse::State& CurrentMouseState;
	};

	DirectX::Mouse::State CurrentMouseState = DirectX::Mouse::Get().GetState();
	FUpdateLastMouseState UpdateLastMouseStateScope {.LastMouseState = LastMouseState, .CurrentMouseState = CurrentMouseState };

	const bool RightMouseButtonDown = CurrentMouseState.rightButton;
	const bool LastRightMouseButtonDown = LastMouseState.rightButton;
	if (!RightMouseButtonDown)
	{
		// 우클릭이 끝난 시점
		if (LastRightMouseButtonDown == true)
		{
			while (ShowCursor(TRUE) < 0);
		}
		return;
	}

	// 이전에는 우클릭 안했다가, 처음 우클릭을 한 경우
	if (LastRightMouseButtonDown == false)
	{
		RightButtonStartMouseState = CurrentMouseState;
		while (ShowCursor(FALSE) >= 0);
	}

	// 변화량을 계산할 수 있으므로, 마우스의 위치를 우클릭을 시작한 위치로 이동한다
	{
		POINT Point = POINT(RightButtonStartMouseState.x, RightButtonStartMouseState.y);
		// 우리 윈도우의 상대 좌표를 Windows의 Screen 좌표로 변환한다
		ClientToScreen(hViewportHandle, &Point);
		SetCursorPos(Point.x, Point.y);
	}

	// UE에서 왼쪽으로 마우스 이동시 -x, 아래로 이동시 -y
	const FVector2D Delta = FVector2D(CurrentMouseState.x - RightButtonStartMouseState.x, RightButtonStartMouseState.y - CurrentMouseState.y);
	FRotator Rot = FRotator::ZeroRotator;

	{
		// MouseDeltaTracker->ConvertMovementDeltaToDragRot
		// -> InViewportClient->ConvertMovementToDragRot
		Rot.Yaw = Delta.x * 0.2f;
		Rot.Pitch = Delta.y * 0.2f;
	}

	// MoveViewportPerspectiveCamera
	{
		FRotator ViewRotation = GetViewRotation();
		ViewRotation += Rot;

		ViewRotation.Yaw = FRotator::NormalizeAxis(ViewRotation.Yaw);
		ViewRotation.Pitch = FRotator::NormalizeAxis(ViewRotation.Pitch);

		// 각도를 ±90도 범위 내로 유지
		ViewRotation.Pitch = FMath::Clamp(ViewRotation.Pitch, -90.f + UE_SMALL_NUMBER, 90.f - UE_SMALL_NUMBER);

		SetViewRotation(ViewRotation);
	}
}

void UEditorViewportClient::CalcSceneView(FSceneViewFamily* ViewFamily)
{
	FRotator ViewRotation = GetViewRotation();

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

	FVector ViewOrigin = GetViewLocation();
	FMatrix ViewTranslationMatrix = FMatrix::CreateTranslation(-ViewOrigin);

	ViewFamily->ViewMatrix = ViewTranslationMatrix * ViewFamily->ViewRotationMatrix;
	ViewFamily->EyePosition = ViewOrigin;

	constexpr float FOV = 90.0;
	constexpr float RadianFOV = DirectX::XMConvertToRadians(FOV);
	const float HalfRadianFOV = RadianFOV / 2.f;

	float XAxisMultiplier;
	float YAxisMultiplier;
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
