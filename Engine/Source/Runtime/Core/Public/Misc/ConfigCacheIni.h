#pragma once
#include "CoreTypes.h"
#include "ConfigFile.h"

class CORE_API FConfigCacheIni
{
public:
	FConfigCacheIni();
	bool LoadIniFile(FStringView InKey, const FString& InIniFileName, const bool bTryOverride);
	FConfigFile& GetConfig(FStringView InKey);

public:
	static FConfigCacheIni* Get(const bool bDestroy = false);

private:
	unordered_map<FString, FConfigFile> CachedConfigMap;
};

CORE_API extern FConfigCacheIni* GConfig;