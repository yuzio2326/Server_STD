#pragma once
#include "Texture.h"
#include "Texture2D.generated.h"

UCLASS()
class ENGINE_API UTexture2D : public UTexture
{
	GENERATED_BODY()
public:
	virtual void LoadTextureFromFile(const FString& InFilePath, const FString& InExtension);
};