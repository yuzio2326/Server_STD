#pragma once
#include "GameFramework/Actor.h"
#include "Camera/CameraTypes.h"
#include "PlayerCameraManager.generated.h"

UCLASS()
class ENGINE_API APlayerCameraManager : public AActor
{
	GENERATED_BODY()
public:
    /** 이 카메라 액터를 소유한 PlayerController입니다. */
    //UPROPERTY(transient)
    class APlayerController* PCOwner = nullptr;

public:
    /** 최소 뷰 피치(도)입니다. */
    UPROPERTY(EditAnywhere/*, BlueprintReadWrite, Category = PlayerCameraManager*/)
    float ViewPitchMin = -89.9f;

    /** 최대 뷰 피치(도)입니다. */
    UPROPERTY(EditAnywhere/*, BlueprintReadWrite, Category = PlayerCameraManager*/)
    float ViewPitchMax = 89.9f;

    /** 최소 뷰 요(도)입니다. */
    UPROPERTY(EditAnywhere/*, BlueprintReadWrite, Category = PlayerCameraManager*/)
    float ViewYawMin = 0.f;

    /** 최대 뷰 요(도)입니다. */
    UPROPERTY(EditAnywhere/*, BlueprintReadWrite, Category = PlayerCameraManager*/)
    float ViewYawMax = 359.999f;

    /** 최소 뷰 롤(도)입니다. */
    UPROPERTY(EditAnywhere/*, BlueprintReadWrite, Category = PlayerCameraManager*/)
    float ViewRollMin = -89.9f;

    /** 최대 뷰 롤(도)입니다. */
    UPROPERTY(EditAnywhere/*, BlueprintReadWrite, Category = PlayerCameraManager*/)
    float ViewRollMax = 89.9f;

public:
    /**
     * 플레이어의 뷰 피치를 제한합니다.
     * @param ViewRotation - 수정할 ViewRotation입니다. 입력 및 출력입니다.
     * @param InViewPitchMin - 최소 뷰 피치(도)입니다.
     * @param InViewPitchMax - 최대 뷰 피치(도)입니다.
     */
    virtual void LimitViewPitch(FRotator& ViewRotation, float InViewPitchMin, float InViewPitchMax);

    /**
     * 플레이어의 뷰 롤을 제한합니다.
     * @param ViewRotation - 수정할 ViewRotation입니다. 입력 및 출력입니다.
     * @param InViewRollMin - 최소 뷰 롤(도)입니다.
     * @param InViewRollMax - 최대 뷰 롤(도)입니다.
     */
    virtual void LimitViewRoll(FRotator& ViewRotation, float InViewRollMin, float InViewRollMax);

    /**
     * 플레이어의 뷰 요를 제한합니다.
     * @param ViewRotation - 수정할 ViewRotation입니다. 입력 및 출력입니다.
     * @param InViewYawMin - 최소 뷰 요(도)입니다.
     * @param InViewYawMax - 최대 뷰 요(도)입니다.
     */
    virtual void LimitViewYaw(FRotator& ViewRotation, float InViewYawMin, float InViewYawMax);

public:
    /**
      * 주어진 관련 PlayerController에 대해 이 PlayerCameraManager를 초기화합니다.
      * @param PC    이 카메라와 관련된 PlayerController입니다.
      */
    virtual void InitializeFor(class APlayerController* PC);

    /**
     * PlayerCameraManager에게 뷰 회전 업데이트가 적용되기 전에 조정할 기회를 주기 위해 호출됩니다.
     * 예를 들어 기본 구현은 LimitViewPitch 등을 사용하여 뷰 회전 한계를 적용합니다.
     * @param DeltaTime - 프레임 시간(초)입니다.
     * @param OutViewRotation - 입출력. 수정할 뷰 회전입니다.
     * @param OutDeltaRot - 입출력. 이 프레임에서 회전이 얼마나 변경되었는지입니다.
     */
    virtual void ProcessViewRotation(float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot);

public:
    /**
     * 매 틱마다 카메라 업데이트를 수행합니다. 모든 다른 액터가 틱 후에 한 번 호출됩니다.
     * 비로컬 플레이어는 bUseClientSideCameraUpdates가 true일 경우 POV를 복제합니다.
     */
    virtual void UpdateCamera(float DeltaTime);

    /** Gets value of CameraCachePrivate.POV */
    virtual const FMinimalViewInfo& GetCameraCacheView() const;

protected:
	FMinimalViewInfo CameraCachePOV;
};