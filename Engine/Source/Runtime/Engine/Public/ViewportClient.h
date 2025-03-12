#pragma once
#include "CoreMinimal.h"
#include "RHIFwd.h"
#include "ViewportClient.generated.h"

class UWorld;
class FSceneInterface;
class UGameInstance;

// Unreal 에서는 FViewport 에서 다양한 ViewportClient(Ex. UGameViewportClient, FEditorViewportClient 등)를 대상으로
// 입력 정보와 렌더링 요청을 전달하고 있는 것으로 보입니다.
// 저희는 이를 간소화 하여 UViewportClient를 만들었고, 여기에 직접적으로 명령을 전달할 예정입니다.
UCLASS()
class ENGINE_API UViewportClient : public UObject
{
	friend class UEngine;
	GENERATED_BODY()
public:
	virtual void Init(HWND hInViewportHandle, UWorld* InWorld);
	virtual void InitPIE(HWND hInViewportHandle, UWorld* InWorld, TEnginePtr<UGameInstance> OwningGameInstance, FViewportRHIRef InViewport);
	virtual void Tick(float DeltaTime);
	virtual void Draw();
	virtual void RequestResize(const uint32 NewSizeX, const uint32 NewSizeY);

	HWND GetNativeWindowHandle() const { return hViewportHandle; }

public:
	/**
	 * @return 이 뷰포트에서 렌더링되는 장면을 반환
	 */
	virtual FSceneInterface* GetScene() const;

protected:
	HWND hViewportHandle = NULL;
	UWorld* World = nullptr;

	FViewportRHIRef Viewport;
	FVector2D ViewportSize;
};