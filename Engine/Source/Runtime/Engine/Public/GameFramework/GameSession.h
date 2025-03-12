#pragma once
#include "GameFramework/Info.h"
#include "GameSession.generated.h"

class APlayerController;

/**
게임 세션 인터페이스 주변의 게임 전용 래퍼 역할을 합니다. 게임 코드는 세션 인터페이스와 상호작용이 필요할 때 이 코드를 호출합니다.
온라인 게임을 실행하는 동안 게임 세션은 서버에만 존재합니다.
*/
UCLASS(/*config = Game, notplaceable, MinimalAPI*/)
class AGameSession : public AInfo
{
	GENERATED_BODY()

public:
    /**
     * GameMode::PostLogin이 호출되어 PostLogin 후 세션 코드가 작업을 수행할 수 있도록 합니다.
     *
     * @param NewPlayer 로그인 중인 플레이어
     */
    virtual void PostLogin(APlayerController* NewPlayer);

};