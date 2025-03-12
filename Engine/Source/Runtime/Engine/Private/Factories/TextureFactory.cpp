#include "Factories/TextureFactory.h"
#include "Engine/Texture2D.h"
#include "Engine/TextureCube.h"

bool UTextureFactory::FactoryCanImport(const FString& Filename)
{
	FString Extension = FPaths::GetExtension(Filename);
	std::transform(Extension.begin(), Extension.end(), Extension.begin(), towlower);
	if (Extension == TEXT("dds") || Extension == TEXT("png"))
	{
        LastExtension = Extension;
		return true;
	}

	return false;
}

TObjectPtr<UObject> UTextureFactory::FactoryCreateFile(const FName InName, const FString& InFileName, const TCHAR* Params)
{
	TObjectPtr<UTexture> NewTexture;
	
	FString ClassName = Params;
	if (ClassName == UTextureCube::StaticClass()->GetName())
	{
		NewTexture = NewObject<UTexture>(nullptr, UTextureCube::StaticClass(), InName);
	}
	else
	{
		NewTexture = NewObject<UTexture>(nullptr, UTexture2D::StaticClass(), InName);
	}

	NewTexture->LoadTextureFromFile(InFileName, LastExtension);

	return NewTexture;
}