#include "SceneView.h"

FSceneViewFamilyContext::~FSceneViewFamilyContext()
{
	// 이 뷰 패밀리에 할당된 뷰들을 정리합니다.
	/*for (int32 ViewIndex = 0; ViewIndex < Views.size(); ViewIndex++)
	{
		delete Views[ViewIndex];
	}*/

}

FSceneViewFamily::ConstructionValues::ConstructionValues(FRHIViewport* InRenderTarget, FSceneInterface* InScene)
	: RenderTarget(InRenderTarget)
	, Scene(InScene)
	, bResolveScene(true)
{
}

FSceneViewFamily::FSceneViewFamily(const ConstructionValues& CVS)
	: RenderTarget(CVS.RenderTarget)
	, Scene(CVS.Scene)
	, bResolveScene(CVS.bResolveScene)
{
}

FSceneViewFamily::~FSceneViewFamily()
{
}
