#include "RHI.h"
#include "CoreMinimal.h"

// Globals.
FDynamicRHI* GDynamicRHI = NULL;

EShadingPath RHI_API GetShadingPath()
{
	static EShadingPath ShadingPath = EShadingPath::Num;
	if (ShadingPath == EShadingPath::Num)
	{
		FString ShadingPathString;
		FConfigFile& ConfigFile = GConfig->GetConfig(GEngineIni);
		ConfigFile.Get("/Script/Engine.RendererSettings", "ShadingPath", ShadingPathString);
		ShadingPath = ShadingPathString == TEXT("Deferred") ? EShadingPath::Deferred : EShadingPath::Forward;
	}
	return ShadingPath;
}

void RHIInit()
{
	if (!GDynamicRHI)
	{
		GDynamicRHI = PlatformCreateDynamicRHI();

		GDynamicRHI->Init();
	}
	_ASSERT(GDynamicRHI);
}
void RHIExit()
{
	if (GDynamicRHI != NULL)
	{
		// Flush any potential commands queued before we shut things down.
		//FRHICommandListExecutor::GetImmediateCommandList().ImmediateFlush(EImmediateFlushType::FlushRHIThread);

		// Destruct the dynamic RHI.
		GDynamicRHI->Shutdown();
		delete GDynamicRHI;
		GDynamicRHI = NULL;
	}
}

FUniformBufferRHIRef RHICreateUniformBuffer(const FConstantBufferInfo& Layout, const void* Contents, const uint32 ContentsSize)
{
	return GDynamicRHI->RHICreateUniformBuffer(Layout, Contents, ContentsSize);
}

FSamplerStateRHIRef RHI_API RHICreateSamplerState(const FSamplerStateInitializerRHI& Initializer)
{
	return GDynamicRHI->RHICreateSamplerState(Initializer);
}

FRasterizerStateRHIRef RHICreateRasterizerState(const FRasterizerStateInitializerRHI& Initializer)
{
	//LLM_SCOPE_BYNAME(TEXT("RHIMisc/CreateRasterizerState"));
	return GDynamicRHI->RHICreateRasterizerState(Initializer);
}

FDepthStencilStateRHIRef RHICreateDepthStencilState(const FDepthStencilStateInitializerRHI& Initializer)
{
	return GDynamicRHI->RHICreateDepthStencilState(Initializer);
}