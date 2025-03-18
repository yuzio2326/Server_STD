#pragma once
#include "EngineMinimal.h"
#include "ServerGameMode.generated.h"

UCLASS()
class GAMEPROJECT_API AServerGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	AServerGameMode();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSceonds) override;

protected:
	std::jthread Thread;
	std::jthread Thread2;
};