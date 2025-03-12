#include "Engine/Texture2D.h"
#include "Engine/AssetManager.h"
#include "RHI.h"

void UTexture2D::LoadTextureFromFile(const FString& InFilePath, const FString& InExtension)
{
	Super::LoadTextureFromFile(InFilePath, InExtension);
	TextureRHI = GDynamicRHI->RHICreateTexture(InFilePath, InExtension);
}
