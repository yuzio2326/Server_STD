#pragma once
#include "GameFramework/Info.h"
#include "PlayerState.generated.h"

UCLASS()
class ENGINE_API APlayerState : public AInfo
{
	GENERATED_BODY()

public:
	void SetPlayerName(const FString& InName) { PlayerName = InName; }
	FString GetPlayerName() const { return PlayerName; }

protected:
	//UPROPERTY(VisibleAnywhere)
	FString PlayerName;
};