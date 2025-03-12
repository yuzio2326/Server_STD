#pragma once
#include "GameFramework/Info.h"
#include "GameStateBase.generated.h"

class AGameModeBase;
//class ASpectatorPawn;
class APlayerState;
class AController;

/**
 * GameStateBase는 게임의 전역 상태를 관리하는 클래스이며, GameModeBase에 의해 생성됩니다.
 * 클라이언트와 서버 양쪽에 존재하며, 완전히 복제됩니다.
 */
UCLASS(/*config = Game, notplaceable, BlueprintType, Blueprintable, MinimalAPI*/)
class AGameStateBase : public AInfo
{
    GENERATED_BODY()

public:
    /** 게임 모드에 의해 호출되어 게임 시작 여부를 나타내는 불리언 값을 설정합니다. */
    virtual void HandleBeginPlay();

    /** 현재 게임 모드의 인스턴스이며, 서버에서만 존재합니다. 비권한 클라이언트의 경우 이 값은 NULL입니다. */
    //UPROPERTY(Transient, BlueprintReadOnly, Category = GameState)
    AGameModeBase* AuthorityGameMode = nullptr;

protected:
    /** GameModeBase->StartPlay가 호출되었을 때 복제되어 클라이언트도 게임을 시작할 수 있도록 합니다. */
    //UPROPERTY(Transient, ReplicatedUsing = OnRep_ReplicatedHasBegunPlay)
    bool bReplicatedHasBegunPlay = false;
};