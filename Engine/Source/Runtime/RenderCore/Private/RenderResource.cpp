#include "RenderResource.h"

RENDERCORE_API TArray<FRenderResource*>& GetRenderResources()
{
	static TArray<FRenderResource*> ResourceList;
	return ResourceList;
}

RENDERCORE_API void FRenderResource::ReleaseRHIForAllResources()
{
	for (FRenderResource* It : GetRenderResources())
	{
		It->ReleaseResource();
	}
}

RENDERCORE_API void FRenderResource::InitPreRHIResources()
{
	for (FRenderResource* It : GetRenderResources())
	{
		It->InitRHI(FRHICommandListExecutor::GetImmediateCommandList());
	}
}

RENDERCORE_API void FRenderResource::InitResource(/*FRHICommandList& RHICmdList*/)
{
	// 이때는 배열에 등록만 해두고 우리는 FRenderResource::InitPreRHIResources();
	// 여기서 초기화 합니다.
	GetRenderResources().push_back(this);
	//InitRHI(RHICmdList);
}

RENDERCORE_API void FRenderResource::ReleaseResource()
{
	TArray<FRenderResource*>& RenderResources = GetRenderResources();
	auto It = find(RenderResources.begin(), RenderResources.end(), this);

	if (It == RenderResources.end())
	{
		return;
	}

	ReleaseRHI();
	GetRenderResources().erase(It);
}

RENDERCORE_API void FVertexBuffer::ReleaseRHI()
{
	VertexBufferRHI.SafeRelease();
}

RENDERCORE_API void FIndexBuffer::ReleaseRHI()
{
	IndexBufferRHI.SafeRelease();
}

void BeginInitResource(FRenderResource* Resource)
{
	Resource->InitResource();
}