#include "DynamicRHI.h"

static FString GLoadedRHIModuleName;

inline const TCHAR* GetLogName(ERHIFeatureLevel::Type InFeatureLevel)
{
	switch (InFeatureLevel)
	{
	case ERHIFeatureLevel::ES3_1: return TEXT("ES3_1");
	case ERHIFeatureLevel::SM5:   return TEXT("SM5");
	case ERHIFeatureLevel::SM6:   return TEXT("SM6");
	default:                      return TEXT("<unknown>");
	}
}

static IDynamicRHIModule* LoadDynamicRHIModule(ERHIFeatureLevel::Type& DesiredFeatureLevel, FString& LoadedRHIModuleName)
{
	FConfigFile& ConfigFile = GConfig->GetConfig(GEngineIni);
	FString RHIName;
	ConfigFile.Get("/Script/WindowsTargetPlatform.WindowsTargetSettings", "DefaultGraphicsRHI", RHIName);
	LoadedRHIModuleName = RHIName;

	IDynamicRHIModule* DynamicRHIModule = FModuleManager::Get()->LoadModulePtr<IDynamicRHIModule>(FName(RHIName));
	if (DynamicRHIModule && DynamicRHIModule->IsSupported(DesiredFeatureLevel))
	{
		E_LOG(Log, TEXT("RHI {} with Feature Level {} is supported and will be used."), LoadedRHIModuleName, GetLogName(DesiredFeatureLevel));

		return DynamicRHIModule;
	}

	E_LOG(Error, TEXT("RHI {} with Feature Level {} is not supported on your system"), LoadedRHIModuleName, GetLogName(DesiredFeatureLevel));

	return nullptr;
}

FDynamicRHI* PlatformCreateDynamicRHI()
{
	FDynamicRHI* DynamicRHI = nullptr;

	ERHIFeatureLevel::Type RequestedFeatureLevel = ERHIFeatureLevel::SM5;
	FString LoadedRHIModuleName;
	IDynamicRHIModule* DynamicRHIModule = LoadDynamicRHIModule(RequestedFeatureLevel, LoadedRHIModuleName);

	if (DynamicRHIModule)
	{
		// Create the dynamic RHI.
		DynamicRHI = DynamicRHIModule->CreateRHI(RequestedFeatureLevel);
		GLoadedRHIModuleName = LoadedRHIModuleName;
	}

	return DynamicRHI;
}
