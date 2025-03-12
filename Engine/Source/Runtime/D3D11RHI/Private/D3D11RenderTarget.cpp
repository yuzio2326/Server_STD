#include "D3D11RHIPrivate.h"

void FD3D11DynamicRHI::RHIBeginRenderPass(const FRHIRenderPassInfo& InInfo, const TCHAR* InName)
{
	FRHISetRenderTargetsInfo RTInfo;
	InInfo.ConvertToRenderTargetsInfo(RTInfo);
	SetRenderTargetsAndClear(RTInfo);

	RenderPassInfo = InInfo;

	//if (InInfo.NumOcclusionQueries > 0)
	//{
	//	RHIBeginOcclusionQueryBatch(InInfo.NumOcclusionQueries);
	//}
}

void FD3D11DynamicRHI::RHIEndRenderPass()
{
	//if (RenderPassInfo.NumOcclusionQueries > 0)
	//{
	//	RHIEndOcclusionQueryBatch();
	//}

	//UE::RHICore::ResolveRenderPassTargets(RenderPassInfo, [this](UE::RHICore::FResolveTextureInfo Info)
	//	{
	//		ResolveTexture(Info);
	//	});

	FRHIRenderTargetView RTV(nullptr, ERenderTargetLoadAction::ENoAction);
	FRHIDepthRenderTargetView DepthRTV(nullptr, ERenderTargetLoadAction::ENoAction, ERenderTargetStoreAction::ENoAction);
	SetRenderTargets(1, &RTV, &DepthRTV);
}