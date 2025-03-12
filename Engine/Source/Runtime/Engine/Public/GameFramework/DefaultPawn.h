#pragma once
#include "GameFramework/Pawn.h"
#include "DefaultPawn.generated.h"

class UStaticMeshComponent;

UCLASS()
class ENGINE_API ADefaultPawn : public APawn
{
	GENERATED_BODY()
public:
	ADefaultPawn();

	virtual void Tick(float DeltaSeconds);

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    /**
	 * 로컬 공간에서 앞으로 이동하기 위한 입력 콜백입니다. (Val이 음수인 경우 뒤로 이동합니다).
	 * @param Val 앞으로 이동하는 양입니다 (음수인 경우 뒤로 이동합니다).
	 * @see APawn::AddMovementInput()
	 */
	//UFUNCTION(BlueprintCallable, Category = "Pawn")
    virtual void MoveForward(float Val);

	/**
	 * 로컬 공간에서 오른쪽으로 이동하는 입력 콜백(Val이 음수일 경우 왼쪽으로 이동).
	 * @param Val 오른쪽으로 이동하는 양(음수일 경우 왼쪽).
	 * @see APawn::AddMovementInput()
	 */
	//UFUNCTION(BlueprintCallable, Category = "Pawn")
	virtual void MoveRight(float Val);

	/**
	 * 로컬 PlayerController인 경우 컨트롤러의 ControlRotation에 입력(Yaw에 영향을 미치는)을 추가합니다.
	 * 이 값은 PlayerController의 InputYawScale 값에 곱해집니다.
	 * @param Val Yaw에 추가할 양입니다. 이 값은 PlayerController의 InputYawScale 값에 곱해집니다.
	 * @see PlayerController::InputYawScale
	 */
	//UFUNCTION(BlueprintCallable, Category = "Pawn|Input", meta = (Keywords = "left right turn addyaw"))
	virtual void AddControllerYawInput(float Val);

	/**
	 * 로컬 PlayerController인 경우 컨트롤러의 ControlRotation에 입력(Pitch에 영향을 미치는)을 추가합니다.
	 * 이 값은 PlayerController의 InputPitchScale 값에 곱해집니다.
	 * @param Val Pitch에 추가할 양입니다. 이 값은 PlayerController의 InputPitchScale 값에 곱해집니다.
	 * @see PlayerController::InputPitchScale
	 */
	//UFUNCTION(BlueprintCallable, Category = "Pawn|Input", meta = (Keywords = "up down addpitch"))
	virtual void AddControllerPitchInput(float Val);

	// BindAction이 맞지만... 그냥
	virtual void ControlYawPith(float Val);

private:
	/** The mesh associated with this Pawn. */
	//UPROPERTY(Category = Pawn, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MeshComponent = nullptr;

	// MovementComponent가 미구현 이므로 Speed 값을 여기에 둠
	UPROPERTY(EditAnywhere)
	float Speed = 200.f;

	// F8을 누르면 회전 풀립니다
	bool bControlYawPith = true;
};