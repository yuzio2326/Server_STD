#pragma once
#include "ViewportClient.h"
#include "GameViewportClient.generated.h"

/**
 * 게임 뷰포트(FViewport)는 플랫폼별 렌더링, 오디오, 입력 하위 시스템을 위한
 * 고수준 추상 인터페이스입니다.
 * GameViewportClient는 게임 뷰포트에 대한 엔진의 인터페이스입니다.
 * 정확히 하나의 GameViewportClient가 게임의 각 인스턴스에 대해 생성됩니다. 엔진의
 * 단일 인스턴스가 있지만 여러 개의 게임 인스턴스(따라서 여러 GameViewportClient)가
 * 있는 경우는 PIE 창이 여러 개 실행 중인 경우뿐입니다.
 *
 * 책임:
 * 입력 이벤트를 전역 상호작용 리스트에 전파합니다.
 *
 * @see UGameViewportClient
 */
UCLASS()
class ENGINE_API UGameViewportClient : public UViewportClient
{
    friend class UEngine;
	GENERATED_BODY()

public:
    virtual void Tick(float DeltaTime) override;
    virtual void Draw() override;

protected:
    /* 이 뷰포트에 대한 상대적인 월드 컨텍스트 */
    //UPROPERTY()
    UWorld* World = nullptr;

    //UPROPERTY()
    UGameInstance* GameInstance = nullptr;
};