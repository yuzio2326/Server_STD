#pragma once
#include "CoreMinimal.h"
#include "Shader.h"
#include "RHIStaticStates.h"

struct FPositionUV
{
	FVector3D Position;
	FVector2D UV;
};
struct FPositionNormalUV
{
	FVector3D Position;
	FVector3D Normal;
	FVector2D UV;
};