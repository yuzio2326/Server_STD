#include "GameMapsSettings.h"

UGameMapsSettings::UGameMapsSettings()
{
	if (!HasAnyFlags(EObjectFlags::RF_ClassDefaultObject)) { _ASSERT(false); return; }

	FConfigFile& ConfigFile = GConfig->GetConfig(GEngineIni);

	{
		FString ClassName;
		ConfigFile.Get("/Script/EngineSettings.GameMapsSettings", "GameInstanceClass", ClassName);
		GameInstanceClass = UClass::FindClass(ClassName);
		ConfigFile.Get("/Script/EngineSettings.GameMapsSettings", "GameModeClass", ClassName);
		GameModeClass = UClass::FindClass(ClassName);
	}
}
