#include "RHICommandList.h"

FRHICommandListExecutor GRHICommandList;

RHI_API void FRHICommandList::InitializeImmediateContexts()
{
	GraphicsContext = ::RHIGetDefaultContext();
}

RHI_API void FRHICommandList::BeginDrawingViewport(FRHIViewport* Viewport, FRHITexture* RenderTargetRHI)
{
	GetContext().RHIBeginDrawingViewport(Viewport, RenderTargetRHI);
}

void FRHICommandList::EndDrawingViewport(FRHIViewport* Viewport, bool bPresent, bool bLockToVsync)
{
	GetContext().RHIEndDrawingViewport(Viewport, bPresent, bLockToVsync);
}

RHI_API void FRHICommandList::SetBoundShaderState(FRHIBoundShaderState* BoundShaderState)
{
	GetContext().RHISetBoundShaderState(BoundShaderState);
}

RHI_API void FRHICommandList::CopyTexture(FRHITexture* SourceTextureRHI, FRHITexture* DestTextureRHI, const FRHICopyTextureInfo& CopyInfo)
{
	GetContext().RHICopyTexture(SourceTextureRHI, DestTextureRHI, CopyInfo);
}

RHI_API void FRHICommandList::SetShaderParameters(FRHIGraphicsShader* InShader, TArray<FRHIShaderParameterResource>& InResourceParameters)
{
	GetContext().RHISetShaderParameters(InShader, InResourceParameters);
}
