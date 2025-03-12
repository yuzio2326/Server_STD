#pragma once
#include "CoreTypes.h"
#include "Private/Misc/inicpp.h"
#include "UObject/UObjectGlobals.h"

class CORE_API FConfigFile
{
public:
	bool Load(FStringView InPath);
	bool TryOverride(FStringView InPath);

	bool Get(string_view InSection, string_view InKey, ini::IniField& OutField);
	
	template<class T>
	bool Get(string_view InSection, string_view InKey, T& OutValue)
	{
		ini::IniField Field;
		bool bFind = Get(InSection, InKey, Field);
		if (!bFind)
		{
			_ASSERT(false);
			return false;
		}

		OutValue = Field.as<T>();
		return true;
	}

	template<>
	bool Get(string_view InSection, string_view InKey, FString& OutString)
	{
		string String;
		if (!Get<string>(InSection, InKey, String))
		{
			return false;
		}

		OutString = ANSI_TO_TCHAR(String);
		return true;
	}
	
private:
	bool Get(ini::IniFile& InFile, string_view InSection, string_view InKey, ini::IniField& OutField);

private:
	bool bLoaded = false;
	bool bOverrided = false;

	ini::IniFile File;
	ini::IniFile OverrideFile;
};