/**
 * 플레이어: 실제 플레이어(로컬 카메라 또는 원격 네트 플레이어)에 해당합니다.
 */

#pragma once

#include "CoreMinimal.h"
#include "Player.generated.h"

UCLASS()
class UPlayer : public UObject
{
	GENERATED_BODY()

public:
	/** 이 플레이어가 제어하는 액터. */
	//UPROPERTY(transient)
	class APlayerController* PlayerController = nullptr;

};