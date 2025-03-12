#pragma once
#include "Texture.h"
#include "TextureCube.generated.h"

UCLASS()
class ENGINE_API UTextureCube : public UTexture
{
	GENERATED_BODY()
public:
	UTextureCube();
	virtual void LoadTextureFromFile(const FString& InFilePath, const FString& InExtension);
};