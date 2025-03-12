#include "Modules/ModuleManager.h"
#include "Logging/Logger.h"
#include "Misc/Paths.h"

FModuleManager::FModuleManager()
{
    ModulePathsCache.insert(make_pair(TEXT("EngineDir"), FPaths::EngineDir()));
}

CORE_API FModuleManager* FModuleManager::Get(const bool bDestroy)
{
    static unique_ptr<FModuleManager> Instance = make_unique<FModuleManager>();
    if (bDestroy)
    {
        for (auto& It : Instance->Modules)
        {
            It.second->Module->ShutdownModule();
            const HMODULE FreeModule = (HMODULE)It.second->Handle;
            const bool bDynamicLoaded = It.second->bDynamicLoaded;
            It.second->Module.reset();
            if (bDynamicLoaded)
            {
                FreeLibrary(FreeModule);
            }
        }

        Instance.reset();
        return nullptr;
    }

    return Instance.get();
}

FModuleManager::ModuleInfoPtr FModuleManager::FindModule(FName InModuleName)
{
    FModuleManager::ModuleInfoPtr Result = nullptr;
    const auto& It = Modules.find(InModuleName);
    if (It == Modules.end())
    {
        return nullptr;
    }

    Result = It->second;

    return Result;
}

IModuleInterface* FModuleManager::LoadModule(const FName InModuleName)
{
    IModuleInterface* LoadedModule = nullptr;
    // 기존 모듈에 대한 빠른 확인을 수행합니다. 이것이 가장 일반적인 경우입니다.
    ModuleInfoPtr FoundModulePtr = FindModule(InModuleName);

    if (FoundModulePtr)
    {
        LoadedModule = FoundModulePtr->Module.get();
        if (LoadedModule)
        {
            // note: this function does not check (bIsReady || IsInGameThread()) the way GetModule() does
            //   that looks like a bug if called from off-game-thread
            return LoadedModule;
        }
    }

    if (!FoundModulePtr)
    {
        AddModule(InModuleName);
        FoundModulePtr = FindModule(InModuleName);
    }

    // Grab the module info.  This has the file name of the module, as well as other info.
    ModuleInfoPtr ModuleInfo = FoundModulePtr;

    auto ModuleInitializerPtr = StaticallyLinkedModuleInitializers.find(InModuleName);
    if (ModuleInitializerPtr != StaticallyLinkedModuleInitializers.end() && ModuleInitializerPtr->second != nullptr)
    {
        // Static lib
        E_LOG(Log, TEXT("ModuleManager: static lib StartupModule module '{}'."), InModuleName.ToString());
        ModuleInfo->Module = unique_ptr<IModuleInterface>(ModuleInitializerPtr->second());
        ModuleInfo->Module->StartupModule();
        LoadedModule = ModuleInfo->Module.get();
    }
    else // DLL
    {
        // Determine which file to load for this module.
        const FString ModuleFileToLoad = ModuleInfo->FullPath;
        if (HMODULE hModule = GetModuleHandle(ModuleFileToLoad.c_str()))
        {
            ModuleInfo->Handle = hModule;
            ModuleInfo->bDynamicLoaded = false;
        }
        else
        {
            ModuleInfo->Handle = LoadLibrary(ModuleFileToLoad.c_str());
            ModuleInfo->bDynamicLoaded = true;
        }
        if (!ModuleInfo->Handle)
        {
            E_LOG(Error, TEXT("can not load module: {}"), InModuleName.ToString())
                return nullptr;
        }

        // Find our "InitializeModule" global function, which must exist for all module DLLs
        FInitializeModuleFunctionPtr InitializeModuleFunctionPtr =
            (FInitializeModuleFunctionPtr)GetProcAddress((HMODULE)ModuleInfo->Handle, "InitializeModule");
        // Initialize the module!
        if (InitializeModuleFunctionPtr != nullptr)
        {
            E_LOG(Log, TEXT("ModuleManager: Load module '{}'."), ModuleFileToLoad);

            ModuleInfo->Module = unique_ptr<IModuleInterface>(InitializeModuleFunctionPtr());
            ModuleInfo->Module->StartupModule();
            LoadedModule = ModuleInfo->Module.get();
        }
        else
        {
            E_LOG(Error, TEXT("ModuleManager: Unable to load module '{}' because InitializeModule function was not found."), ModuleFileToLoad);

            FreeLibrary((HMODULE)ModuleInfo->Handle);
            ModuleInfo->Handle = nullptr;
        }
    }

    return LoadedModule;
}

CORE_API IModuleInterface& FModuleManager::LoadModuleChecked(const FName InModuleName)
{
    IModuleInterface* Module = LoadModule(InModuleName/*, ELoadModuleFlags::LogFailures*/);
    _ASSERT(Module, TEXT("%s"), *InModuleName.ToString());

    return *Module;
}

CORE_API void FModuleManager::AddModule(const FName InModuleName)
{
    if (Modules.contains(InModuleName))
    {
        E_LOG(Error, TEXT("check add module {}"), InModuleName.ToString());
        return;
    }
    ModuleInfoPtr ModuleInfo(new FModuleInfo());

    Modules.emplace(InModuleName, ModuleInfo);

    bool bFound = false;
    const FString SearchModuleName = InModuleName.ToString();
#if WITH_EDITOR
    ModuleInfo->OriginalFilename = SearchModuleName + TEXT(".dll"); // no extension Ex) A.dll
    for (auto ItPath : ModulePathsCache)
    {
        if (filesystem::is_directory(ItPath.second))
        {
            for (const auto& It : filesystem::directory_iterator(ItPath.second))
            {
                if (It.path().extension() == TEXT(".dll") && It.path().stem() == SearchModuleName)
                {
                    bFound = true;
                    ModuleInfo->FullPath = It.path(); // 로드를 위한 전체 경로
                    break;
                }
            }
        }
    }
#else
    bFound = true;
#endif

    if (!bFound)
    {
        E_LOG(Error, TEXT("{} module not found"), SearchModuleName);
    }
}