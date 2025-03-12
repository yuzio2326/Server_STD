#pragma once
#include "CoreMinimal.h"
#include "RHIFwd.h"
#include "Texture.generated.h"

UCLASS(/*abstract*/)
class ENGINE_API UTexture : public UObject
{
	GENERATED_BODY()

public:
	virtual void LoadTextureFromFile(const FString& InFilePath, const FString& InExtension) {}

public:
	FTextureRHIRef GetTextureRHI() const { return TextureRHI; }

protected:
	FTextureRHIRef TextureRHI;
};