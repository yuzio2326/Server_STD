#pragma once
#include "GameFramework/Actor.h"
#include "Controller.generated.h"

class ACharacter;
class APawn;
class APlayerState;

/**
 * 컨트롤러는 Pawn을 소유하여 그 행동을 제어할 수 있는 비물리적 액터입니다. PlayerController는 인간 플레이어가 Pawn을 제어하는 데 사용되고,
 * AIController는 제어하는 Pawn에 대한 인공지능을 구현합니다.
 * 컨트롤러는 Possess() 메서드를 사용하여 Pawn을 제어하며, UnPossess()를 호출하여 Pawn의 제어를 포기합니다.
 *
 * 컨트롤러는 제어하는 Pawn에서 발생하는 많은 이벤트에 대한 알림을 받습니다. 이를 통해 컨트롤러는 이 이벤트에 응답하여 동작을 구현하고, 이벤트를 가로채서 Pawn의 기본 동작을 대신 실행할 수 있습니다.
 *
 * ControlRotation (GetControlRotation()을 통해 액세스)은 제어하는 Pawn의 시야/조준 방향을 결정하며, 마우스나 게임패드와 같은 입력에 영향을 받습니다.
 *
 * @see https://docs.unrealengine.com/latest/INT/Gameplay/Framework/Controller/
 */
UCLASS(/*abstract, notplaceable, NotBlueprintable, HideCategories = (Collision, Rendering, Transformation), MinimalAPI*/)
class ENGINE_API AController : public AActor//, public INavAgentInterface
{
    GENERATED_BODY()

public:
    /**
     * 컨트롤 회전을 가져옵니다. 이것은 전체 조준 회전이며, 카메라 방향과 다를 수 있습니다 (예: 3인칭 뷰).
     * 그리고 제어된 폰의 회전과 다를 수 있습니다 (예: 시각적으로 pitch나 roll을 선택하지 않을 수 있음).
     */
    //UFUNCTION(BlueprintCallable, Category = Pawn)
    virtual FRotator GetControlRotation() const;

public:
    /** 이 컨트롤러를 사용하는 플레이어에 대한 복제된 정보를 포함하는 PlayerState입니다 (NPC에는 존재하지 않습니다). */
    //UPROPERTY(replicatedUsing = OnRep_PlayerState, BlueprintReadOnly, Category = Controller)
    TEnginePtr<APlayerState> PlayerState;

public:
    /** Pawn의 설정자입니다. 일반적으로 Pawn을 소유하거나 소유 해제할 때 내부적으로만 사용해야 합니다. */
    virtual void SetPawn(APawn* InPawn);

    /** Getter for Pawn */
    FORCEINLINE APawn* GetPawn() const { return Pawn; }

    /** Templated version of GetPawn, will return nullptr if cast fails */
    template<class T>
    T* GetPawn() const
    {
        return Cast<T>(Pawn);
    }

    /**
     * 이 컨트롤러를 지정된 폰에 연결하는 작업을 처리합니다.
     * 네트워크 권한이 있는 경우에만 실행됩니다(HasAuthority()가 true를 반환하는 경우).
     * 파생된 네이티브 클래스는 OnPossess를 재정의하여 지정된 폰을 필터링할 수 있습니다.
     * 소유한 폰이 변경되면 블루프린트 클래스는 ReceivePossess에 의해 알림을 받고
     * OnNewPawn 델리게이트가 브로드캐스트됩니다.
     * @param InPawn 소유할 폰입니다.
     * @see HasAuthority, OnPossess, ReceivePossess
     */
    //UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Pawn, meta = (Keywords = "set controller"))
    virtual void Possess(APawn* InPawn) final; // DEPRECATED(4.22, "Possess is marked virtual final as you should now be overriding OnPossess instead")

protected:
    /**
     * 이 컨트롤러가 폰을 소유하도록 요청받을 때 호출되는 재정의 가능한 네이티브 함수입니다.
     * @param InPawn 소유할 폰입니다.
     */
    virtual void OnPossess(APawn* InPawn);

protected:
    /** 컨트롤러의 컨트롤 회전입니다. GetControlRotation을 참조하십시오. */
    UPROPERTY()
    FRotator ControlRotation;

private:
    /** 이 컨트롤러가 현재 제어 중인 폰입니다. 폰을 제어하려면 Pawn.Possess()를 사용하십시오. */
    //UPROPERTY(replicatedUsing = OnRep_Pawn)
    TEnginePtr<APawn> Pawn;

};