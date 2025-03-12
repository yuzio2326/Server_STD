#pragma once
#include "CoreTypes.h"

class CORE_API FPaths
{
public:
	// exe 경로
	static FString EngineDir();
	static FString EngineConfigDir();
	static FString ContentDir();
	static FString ShaderDir();

	static FString GetExtension(const FString& InPath, bool bIncludeDot = false);
};