#include "Editor/CameraController.h"

ENGINE_API void FEditorCameraController::UpdateSimulation(
	const FCameraControllerUserImpulseData& UserImpulseData, 
	const float DeltaTime, const float MovementSpeedScale,
	FVector& InOutCameraPosition,
	FRotator& InOutCameraRotation)
{
	// Movement
	UpdatePosition(UserImpulseData, DeltaTime, MovementSpeedScale, 
		InOutCameraRotation, InOutCameraPosition);
}

void FEditorCameraController::UpdatePosition(const FCameraControllerUserImpulseData& UserImpulse, const float DeltaTime, const float MovementSpeedScale, const FRotator& CameraRotation, FVector& InOutCameraPosition)
{
	// 로컬 임펄스를 계산합니다.
	FVector LocalSpaceImpulse;
	{
		// 참고: 앞/뒤 및 좌/우 임펄스는 로컬 공간에서 적용되지만, 위/아래 임펄스는
		//       월드 공간에서 적용됩니다. 이는 해당 컨트롤로 항상 직선으로 위아래로
		//       이동하는 것이 더 직관적이기 때문입니다.
		LocalSpaceImpulse =
			FVector(UserImpulse.MoveForwardBackwardImpulse,		// 로컬 공간의 앞/뒤
				UserImpulse.MoveRightLeftImpulse,			// 로컬 공간의 좌/우
				0.0f);										// 로컬 공간의 위/아래

		// DirectX 기본
		/*LocalSpaceImpulse =
			FVector(UserImpulse.MoveRightLeftImpulse,
				0.f,			
				-UserImpulse.MoveForwardBackwardImpulse);*/
	}

	// 월드 공간 가속도를 계산합니다.
	FVector WorldSpaceAcceleration;
	{
		// 카메라 방향을 계산한 후 로컬 공간 임펄스를 월드 공간으로 회전시킵니다.
		const FQuat CameraOrientation = CameraRotation.Quaternion();
		FVector WorldSpaceImpulse = FVector::Transform(LocalSpaceImpulse, CameraOrientation); // 벡터를 특정 회전값을 사용해서 회전 시킨다

		// 월드 공간 임펄스를 누적합니다.
		// 참고: 위/아래 임펄스는 월드 공간에서 적용됩니다. 자세한 내용은 위의 주석을 참조하세요.
		WorldSpaceImpulse +=
			FVector(0.0f,											// 월드 공간의 앞/뒤
				0.0f,											// 월드 공간의 좌/우
				UserImpulse.MoveUpDownImpulse);				// 월드 공간의 위/아래

		/*WorldSpaceImpulse +=
			FVector(0.0f,
				UserImpulse.MoveUpDownImpulse,
				0.0f);*/

		// 임펄스의 크기가 1.0보다 큰 경우 정규화하여 제한합니다.
		//if (WorldSpaceImpulse.SizeSquared() > 1.0f)
		{
			//WorldSpaceImpulse = WorldSpaceImpulse.UnsafeNormal();
		}

		// 월드 공간 가속도를 계산합니다.
		WorldSpaceAcceleration = WorldSpaceImpulse * /*Config.MovementAccelerationRate **/ MovementSpeedScale;
	}

	{
		// 이동 속도를 가속합니다.
		MovementVelocity += WorldSpaceAcceleration * DeltaTime;

		// 감쇠 적용
		{
			const float MovementVelocityDampingAmount = 10.f;
			const float DampingFactor = FMath::Clamp(MovementVelocityDampingAmount * DeltaTime, 0.0f, 0.75f);

			// 감속
			MovementVelocity += -MovementVelocity * DampingFactor;
		}
	}

	// 속도를 적절히 작은 숫자로 클램프합니다.
	if (MovementVelocity.LengthSquared() < UE_SMALL_NUMBER * UE_SMALL_NUMBER)
	{
		MovementVelocity = FVector::Zero;
	}

	// 카메라 위치를 업데이트합니다.
	InOutCameraPosition += MovementVelocity * DeltaTime;
}
