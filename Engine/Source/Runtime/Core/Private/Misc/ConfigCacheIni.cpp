#include "Misc/ConfigCacheIni.h"
#include "Logging/Logger.h"
#include "Misc/Paths.h"
#include "Misc/Names.h"

FConfigCacheIni* GConfig = nullptr;

FConfigCacheIni::FConfigCacheIni()
{
	LoadIniFile(GEngineIni, GBaseEngineIni, false);
	LoadIniFile(GEngineIni, GDefaultEngineIni, true);
}

bool FConfigCacheIni::LoadIniFile(FStringView InKey, const FString& InIniFileName, const bool bTryOverride)
{
	FConfigFile& ConfigFile = CachedConfigMap[InKey.data()];

	const FString IniFilePath = FPaths::EngineConfigDir() + TEXT("\\") + InIniFileName;
	bool bLoaded = false;

	if (!bTryOverride)
	{
		bLoaded = ConfigFile.Load(IniFilePath);
		if (!bLoaded)
		{
			E_LOG(Error, TEXT("Ini file load failed: {}"), IniFilePath);
		}
	}
	else
	{
		bLoaded = ConfigFile.TryOverride(IniFilePath);
	}

	return bLoaded;
}

FConfigFile& FConfigCacheIni::GetConfig(FStringView InKey)
{
	if (!CachedConfigMap.contains(InKey.data()))
	{
		E_LOG(Error, TEXT("{} Config File not found"), InKey);
	}

	return CachedConfigMap[InKey.data()];
}

FConfigCacheIni* FConfigCacheIni::Get(const bool bDestroy)
{
	static unique_ptr<FConfigCacheIni> Instance = make_unique<FConfigCacheIni>();
	if (bDestroy)
	{
		Instance.reset();
		GConfig = nullptr;
		return nullptr;
	}
	if (!GConfig)
	{
		GConfig = Instance.get();
	}
	return Instance.get();
}
