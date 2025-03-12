#include "Engine/TextureCube.h"
#include "Engine/AssetManager.h"
#include "RHI.h"

UTextureCube::UTextureCube()
{
	if(!HasAnyFlags(EObjectFlags::RF_ClassDefaultObject))
	{
		return;
	}
	
#if !SERVER
	FAssetManager::Get()->LoadAsset<UTextureCube>(FPaths::ContentDir() + L"/Engine/Textures/SunSubMixer_diffuseIBL.dds", UTextureCube::StaticClass()->GetName().c_str());
	FAssetManager::Get()->LoadAsset<UTextureCube>(FPaths::ContentDir() + L"/Engine/Textures/SunSubMixer_specularIBL.dds", UTextureCube::StaticClass()->GetName().c_str());
#endif
}

void UTextureCube::LoadTextureFromFile(const FString& InFilePath, const FString& InExtension)
{
	Super::LoadTextureFromFile(InFilePath, InExtension);
	// see UE: FTextureCubeResource
	TextureRHI = GDynamicRHI->RHICreateCubeTexture(InFilePath, InExtension);
}
