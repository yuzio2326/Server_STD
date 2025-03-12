#include "EngineModule.h"
#include "RendererInterface.h"

IRendererModule* CachedRendererModule = NULL;

ENGINE_API IRendererModule& GetRendererModule()
{
	if (!CachedRendererModule)
	{
		CachedRendererModule = &FModuleManager::LoadModuleChecked<IRendererModule>(TEXT("Renderer"));
	}

	return *CachedRendererModule;
}