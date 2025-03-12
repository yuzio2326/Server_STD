#pragma once
#include "CoreMinimal.h"
#include "GameMapsSettings.generated.h"


UCLASS()
class ENGINE_API UGameMapsSettings : public UObject
{
	GENERATED_BODY()

public:
	UGameMapsSettings();

	UClass* GameInstanceClass = nullptr;
	UClass* GameModeClass = nullptr;
};