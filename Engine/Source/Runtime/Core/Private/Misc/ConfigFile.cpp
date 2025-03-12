#include "Misc/ConfigFile.h"
#include "Logging/Logger.h"
#include "UObject/UObjectGlobals.h"

bool FConfigFile::Load(FStringView InPath)
{
	if (bLoaded)
	{
		E_LOG(Error, TEXT("Config file was loaded: {}"), InPath);
		return false;
	}

	bLoaded =  File.load(TCHAR_TO_ANSI(InPath));
	_ASSERT(bLoaded);
	return bLoaded;
}

bool FConfigFile::TryOverride(FStringView InPath)
{
	if (!bLoaded)
	{
		E_LOG(Error, TEXT("bLoaded is false: {}"), InPath);
		return false;
	}

	if (bOverrided)
	{
		E_LOG(Error, TEXT("bOverrided is true: {}"), InPath);
		return false;
	}

	bOverrided = OverrideFile.load(TCHAR_TO_ANSI(InPath));
	return bOverrided;
}

bool FConfigFile::Get(string_view InSection, string_view InKey, ini::IniField& OutField)
{
	if (bOverrided)
	{
		bool bFind = Get(OverrideFile, InSection, InKey, OutField);
		if (bFind) { return true; }
	}
	return Get(File, InSection, InKey, OutField);
}

bool FConfigFile::Get(ini::IniFile& InFile, string_view InSection, string_view InKey, ini::IniField& OutField)
{
	if (!InFile.contains(InSection.data()))
	{
		return false;
	}

	ini::IniSection& Section = InFile[InSection.data()];
	if (!Section.contains(InKey.data()))
	{
		return false;
	}

	OutField = Section[InKey.data()];

	return true;
}
