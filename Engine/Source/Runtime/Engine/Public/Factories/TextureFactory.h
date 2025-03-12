#pragma once
#include "Factory.h"
#include "TextureFactory.generated.h"

UCLASS()
class UTextureFactory : public UFactory
{
	GENERATED_BODY()

public:
	virtual bool FactoryCanImport(const FString& Filename) override;
	virtual TObjectPtr<UObject> FactoryCreateFile(const FName InName, const FString& InFileName, const TCHAR* Params) override;

protected:
	FString LastExtension;
};