#pragma once
#include "CoreMinimal.h"

class FShaderCompilingManager
{
public:
	static ENGINE_API FShaderCompilingManager* Get(const bool bDestroy = false);
	FShaderCompilingManager();
};