#include "Engine/AssetManager.h"
#include "Engine/Engine.h"
#include "Factories/Factory.h"

FAssetManager* FAssetManager::Get(const bool bDestroy)
{
    static unique_ptr<FAssetManager> Instance = make_unique<FAssetManager>();
    if (bDestroy)
    {
        Instance.reset();
        return nullptr;
    }

    return Instance.get();
}

TEnginePtr<UObject> FAssetManager::LoadAsset(const type_info& InAssetType, const FString& InFilePath, const TCHAR* Params)
{
    if (LoadedAssets[InAssetType.hash_code()].contains(InFilePath))
    {
        return LoadedAssets[InAssetType.hash_code()][InFilePath];
    }

    const bool bFileExist = std::filesystem::exists(InFilePath);
    if (!bFileExist)
    {
        E_LOG(Error, TEXT("Can not find file path: {}"), InFilePath)
        return nullptr;
    }

    TArray<UClass*> FactoryClasses = UClass::GetAllSubclassOfClass(UFactory::StaticClass());
    UFactory* NewFactory = nullptr;
    for (UClass* Class : FactoryClasses)
    {
        UFactory* Factory = Class->GetDefaultObject<UFactory>();
        if (Factory == nullptr)
        {
            E_LOG(Error, TEXT("CDO 로딩 중"));
            return nullptr;
        }

        if (Factory->FactoryCanImport(InFilePath))
        {
            NewFactory = Factory;
            break;
        }
    }

    if (NewFactory)
    {
        TObjectPtr<UObject> NewAsset = NewFactory->FactoryCreateFile(InFilePath, InFilePath, Params);
        LoadedAssets[InAssetType.hash_code()][InFilePath] = NewAsset;
        return NewAsset;
    }

    _ASSERT(false);
    return nullptr;
}