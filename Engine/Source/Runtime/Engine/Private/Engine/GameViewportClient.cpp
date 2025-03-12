#include "Engine/GameViewportClient.h"
#include "RenderResource.h"
#include "RendererInterface.h"
#include "SceneView.h"
#include "EngineModule.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Engine/LocalPlayer.h"
#include "Engine/GameInstance.h"

void UGameViewportClient::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void UGameViewportClient::Draw()
{
	if (!GetScene()) { return; }
	// 뷰포트를 위한 FSceneViewFamily/FSceneView 설정
	FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(Viewport, GetScene()));

	// 플레이어의 뷰 정보를 계산합니다.
	FVector     ViewLocation = FVector::Zero;
	FRotator    ViewRotation = FRotator::ZeroRotator;

	ULocalPlayer* LocalPlayer = GameInstance->GetLocalPlayer(0);
	ViewFamily.ViewportSize = ViewportSize;
	LocalPlayer->CalcSceneView(&ViewFamily, ViewLocation, ViewRotation);

	GetRendererModule().BeginRenderingViewFamily(&ViewFamily);
}