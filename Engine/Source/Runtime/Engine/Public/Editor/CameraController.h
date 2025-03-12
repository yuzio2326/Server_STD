#pragma once

/*=============================================================================
    CameraController.h: 의사 물리학을 적용한 카메라 제어 구현
=============================================================================*/

#pragma once

#include "CoreMinimal.h"

/**
 * FCameraControllerUserImpulseData
 *
 * 카메라 움직임을 위한 다양한 사용자 입력 매개변수의 래퍼 구조체
 */
class FCameraControllerUserImpulseData
{

public:

    /** 전진(양수) 또는 후진(음수) 이동을 위한 스칼라 사용자 입력 */
    float MoveForwardBackwardImpulse;

    /** 오른쪽(양수) 또는 왼쪽(음수) 이동을 위한 스칼라 사용자 입력 */
    float MoveRightLeftImpulse;

    /** 위로(양수) 또는 아래로(음수) 이동을 위한 스칼라 사용자 입력 */
    float MoveUpDownImpulse;

    /** 오른쪽(양수) 또는 왼쪽(음수) 회전을 위한 스칼라 사용자 입력 */
    float RotateYawImpulse;

    /** 위로(양수) 또는 아래로(음수) 피치를 위한 스칼라 사용자 입력 */
    float RotatePitchImpulse;

    /** 시계 방향(양수) 또는 반시계 방향(음수) 롤을 위한 스칼라 사용자 입력 */
    float RotateRollImpulse;

    /** 오른쪽(양수) 또는 왼쪽(음수) 회전을 위한 속도 수정자 */
    float RotateYawVelocityModifier;

    /** 위로(양수) 또는 아래로(음수) 피치를 위한 속도 수정자 */
    float RotatePitchVelocityModifier;

    /** 시계 방향(양수) 또는 반시계 방향(음수) 롤을 위한 속도 수정자 */
    float RotateRollVelocityModifier;

    ///** FOV 증가(줌 아웃, 양수) 또는 감소(줌 인, 음수)를 위한 스칼라 사용자 입력 */
    //float ZoomOutInImpulse;


    /** 생성자 */
    FCameraControllerUserImpulseData()
        : MoveForwardBackwardImpulse(0.0f),
        MoveRightLeftImpulse(0.0f),
        MoveUpDownImpulse(0.0f),
        RotateYawImpulse(0.0f),
        RotatePitchImpulse(0.0f),
        RotateRollImpulse(0.0f),
        RotateYawVelocityModifier(0.0f),
        RotatePitchVelocityModifier(0.0f),
        RotateRollVelocityModifier(0.0f)
    {
    }
};


/**
 * FEditorCameraController
 *
 * 상호작용 카메라 이동 시스템. 간단한 물리 기반 애니메이션을 지원합니다.
 */
class FEditorCameraController
{
public:
    virtual ~FEditorCameraController() {}

    /**
     * 카메라의 위치와 방향 및 속도와 같은 다른 상태를 업데이트합니다. 매 프레임마다 호출되어야 합니다.
     *
     * @param	UserImpulseData			이 프레임에서 사용자의 입력 데이터
     * @param	DeltaTime				마지막 업데이트 이후의 시간 간격
     * @param	MovementSpeedScale		이동 속도를 조정하는 스케일
     * @param	InOutCameraPosition		[in, out] 카메라 위치
     */
    ENGINE_API virtual void UpdateSimulation(
        const FCameraControllerUserImpulseData& UserImpulseData,
        const float DeltaTime,
        const float MovementSpeedScale,
        FVector& InOutCameraPosition, 
        FRotator& InOutCameraRotation);

    /**
     * 카메라 위치를 업데이트합니다. UpdateSimulation에 의해 매 프레임마다 호출됩니다.
     *
     * @param	UserImpulse				현재 프레임의 사용자 임펄스 데이터
     * @param	DeltaTime				시간 간격
     * @param	MovementSpeedScale		추가 이동 가속/속도 스케일
     * @param	CameraEuler				현재 카메라 회전 (FRotator로 우리는 바꿈)
     * @param	InOutCameraPosition		[in, out] 카메라 위치
     */
    void UpdatePosition(const FCameraControllerUserImpulseData& UserImpulse,
        const float DeltaTime, const float MovementSpeedScale, 
        const FRotator& CameraRotation, FVector& InOutCameraPosition);

private:
    /** 월드 공간 이동 속도 */
    FVector MovementVelocity;
};