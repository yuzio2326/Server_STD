#pragma once
#include "GameFramework/Actor.h"
#include "Pawn.generated.h"

class AController;

UCLASS()
class ENGINE_API APawn : public AActor
{
	GENERATED_BODY()

public:
	virtual void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const;
	
	/** Returns	Pawn's eye location */
	virtual FVector GetPawnViewLocation() const;
	/**
	 * Pawn의 뷰 회전(보는 방향, 일반적으로 Controller->ControlRotation)을 가져옵니다.
	 * @return Pawn의 뷰 회전입니다.
	 */
	virtual FRotator GetViewRotation() const;

public:
	/** 충돌 중심 위의 기본 눈 높이입니다. */
	UPROPERTY(EditAnywhere/*, BlueprintReadWrite, Category = Camera*/)
	float BaseEyeHeight = 0.f;

public:
	/** Returns controller for this actor. */
	//UFUNCTION(BlueprintCallable, Category = Pawn)
	AController* GetController() const;

	/**
	 * 이 Pawn이 소유될 때 호출됩니다. 서버(또는 독립 실행형)에서만 호출됩니다.
	 * @param NewController 이 Pawn을 소유하는 컨트롤러입니다.
	 */
	virtual void PossessedBy(AController* NewController);

	/** 올바른 재시작 함수를 호출하기 위한 래퍼 함수입니다. 로컬 소유 플레이어 폰 또는 이에 상응하는 경우 bCallClientRestart를 활성화합니다. */
	void DispatchRestart(bool bCallClientRestart);

	/** 플레이어가 제어하는 폰이 재시작될 때 소유 클라이언트에서 호출됩니다. 이는 Restart()를 호출합니다. */
	virtual void PawnClientRestart();

	/** 폰이 재시작될 때 호출됩니다 (주로 컨트롤러에 의해 소유될 때). 모든 폰에 대해 서버에서 호출되며, 플레이어 폰에 대해서는 소유 클라이언트에서 호출됩니다. */
	virtual void Restart();

protected:
	/** 맞춤 입력 바인딩에 사용할 수 있는 InputComponent를 생성합니다. PlayerController에 의해 소유될 때 호출됩니다. 원하지 않으면 null을 반환합니다. */
	virtual TEnginePtr<UInputComponent> CreatePlayerInputComponent();

	/** Pawn이 맞춤 입력 바인딩을 설정할 수 있도록 합니다. CreatePlayerInputComponent()에 의해 생성된 InputComponent를 사용하여 PlayerController에 의해 소유될 때 호출됩니다. */
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) { /* 기본적으로 바인딩이 없습니다. */ }

public:
	/** Controller currently possessing this Actor */
	//UPROPERTY(replicatedUsing = OnRep_Controller)
	TEnginePtr<AController> Controller;
};