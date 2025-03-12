#pragma once
#include "GameFramework/Controller.h"
#include "Camera/PlayerCameraManager.h"
#include "PlayerController.generated.h"

class APawn;
class UInputComponent;
class UPlayer;
class UPlayerInput;
class UPrimitiveComponent;

/**
 * PlayerController는 인간 플레이어가 Pawns를 제어하는 데 사용됩니다.
 *
 * ControlRotation (GetControlRotation()을 통해 액세스)은 제어된 Pawn의 조준 방향을 결정합니다.
 *
 * 네트워크 게임에서, PlayerControllers는 모든 플레이어가 제어하는 Pawn에 대해 서버에 존재하며,
 * 또한 제어하는 클라이언트의 기기에서도 존재합니다. 다른 네트워크의 원격 플레이어가 제어하는 Pawn에 대해서는
 * 클라이언트 기기에 존재하지 않습니다.
 *
 * @see https://docs.unrealengine.com/latest/INT/Gameplay/Framework/Controller/PlayerController/
 */
UCLASS(/*config = Game, BlueprintType, Blueprintable, meta = (ShortTooltip = "Player Controller는 플레이어가 사용하는 Pawn을 제어하는 역할을 하는 액터입니다."), MinimalAPI*/)
class ENGINE_API APlayerController : public AController//, public IWorldPartitionStreamingSourceProvider
{
    GENERATED_BODY()

public:
    // ******************************************************************************
    // 카메라/뷰 관련 변수들

    /** 이 플레이어 컨트롤러에 연결된 카메라 매니저입니다. */
    //UPROPERTY(BlueprintReadOnly, Category = PlayerController)
    APlayerCameraManager* PlayerCameraManager = nullptr;

    /** 각 게임마다 PlayerCamera 클래스를 설정해야 하며, 그렇지 않으면 Engine.PlayerCameraManager가 사용됩니다. */
    //UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = PlayerController)
    TSubclassOf<APlayerCameraManager> PlayerCameraManagerClass;

public:
    /**
     * Yaw(회전) 입력을 추가합니다. 이 값은 InputYawScale에 곱해집니다.
     * @param Val Yaw에 추가할 양입니다. 이 값은 InputYawScale에 곱해집니다.
     */
    //UFUNCTION(BlueprintCallable, Category = "Game|Player", meta = (Keywords = "left right turn"))
    virtual void AddYawInput(float Val);

    /**
     * Pitch(위/아래 보기) 입력을 추가합니다. 이 값은 InputPitchScale에 곱해집니다.
     * @param Val Pitch에 추가할 양입니다. 이 값은 InputPitchScale에 곱해집니다.
     */
    //UFUNCTION(BlueprintCallable, Category = "Game|Player", meta = (Keywords = "up down"))
    virtual void AddPitchInput(float Val);

    /**
     * RotationInput이 적용된 후 ControlRotation을 기반으로 플레이어의 회전을 업데이트합니다.
     * 이 값은 PlayerCamera에 의해 수정될 수 있으며, Pawn->FaceRotation()에 전달됩니다.
     */
    virtual void UpdateRotation(float DeltaTime);

public:
    virtual void PostInitializeComponents() override;

    /** 이 컨트롤러에 대한 PlayerState를 생성하고 초기화합니다. */
    virtual void InitPlayerState();

    virtual void Tick(float DeltaSeconds);

    /** 서버 및 소유한 플레이어를 위한 카메라를 생성합니다. */
    virtual void SpawnPlayerCameraManager();

    /** 카메라 매니저를 업데이트합니다. 모든 액터가 틱 후 호출됩니다. */
    virtual void UpdateCameraManager(float DeltaSeconds);

public:
    /** 이 PlayerController에 새 UPlayer를 연결합니다. */
    virtual void SetPlayer(UPlayer* InPlayer);

    /** 이 PlayerController와 연결된 UPlayer입니다. 로컬 플레이어일 수도 있고 네트워크 연결일 수도 있습니다. */
    //UPROPERTY()
    //TObjectPtr<UPlayer> Player;
    UPlayer* Player = nullptr;

    /** 플레이어 입력을 관리하는 객체입니다. */
    //UPROPERTY(Transient)
    TObjectPtr<UPlayerInput> PlayerInput;

    /** 매 틱마다 누적된 입력 축 값입니다. */
    FRotator RotationInput;

public:
    /**
     * 적절한 클래스의 PlayerInput을 생성합니다.
     * 로컬 플레이어에 속한 플레이어 컨트롤러에 대해서만 호출됩니다.
     */
    virtual void InitInputSystem();

public:
    /** 제어 회전을 설정합니다. RootComponent->bAbsoluteRotation이 true인 경우, RootComponent의 회전도 일치하도록 업데이트됩니다. */
    //UFUNCTION(BlueprintCallable, Category = Pawn, meta = (Tooltip = "Set the control rotation."))
    /*ENGINE_API*/ virtual void SetControlRotation(const FRotator& NewRotation);

protected:
    /** PlayerController가 사용자 정의 입력 바인딩을 설정할 수 있도록 합니다. */
    virtual void SetupInputComponent();

    void TickPlayerInput(const float DeltaSeconds, const bool bGamePaused);
    virtual void ProcessPlayerInput(const float DeltaTime, const bool bGamePaused);
    virtual void BuildInputStack(TArray<UInputComponent*>& InputStack);

protected:
    virtual void OnPossess(APawn* InPawn);

    /** Tell client to restart the level */
    //UFUNCTION(Reliable, Client)
    void ClientRestart(class APawn* NewPawn);

public:
    /** 입력이 활성화된 경우, 이 액터의 입력을 처리하는 컴포넌트입니다. */
    //UPROPERTY(/*DuplicateTransient*/)
    TObjectPtr<class UInputComponent> InputComponent;
};