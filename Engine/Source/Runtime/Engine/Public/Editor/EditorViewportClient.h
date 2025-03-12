#pragma once
#include "ViewportClient.h"
#include "EditorViewportClient.generated.h"

class FCameraControllerUserImpulseData;
class FEditorCameraController;
class FSceneViewFamily;
/**
 * 뷰포트 카메라의 변환 데이터를 저장합니다.
 */
struct ENGINE_API FViewportCameraTransform
{
public:
    /** 변환의 위치를 설정합니다. */
	FORCEINLINE void SetLocation(const FVector& Position) { ViewLocation = Position; }

    /** 변환의 회전을 설정합니다. */
	FORCEINLINE void SetRotation(const FRotator& Rotation) { ViewRotation = Rotation; }

    /** @return 변환의 위치를 반환합니다. */
    FORCEINLINE const FVector& GetLocation() const { return ViewLocation; }

    /** @return 변환의 회전을 반환합니다. */
    FORCEINLINE const FRotator& GetRotation() const { return ViewRotation; }

private:
    /** 현재 뷰포트 위치 */
    FVector	ViewLocation = FVector::Zero;
    /** 현재 뷰포트 방향; 원근 투영에 대해서만 유효합니다. */
    FRotator ViewRotation = FRotator::ZeroRotator;
};

// Unreal 에서는 FViewport 에서 다양한 ViewportClient(Ex. UGameViewportClient, FEditorViewportClient 등)를 대상으로
// 입력 정보와 렌더링 요청을 전달하고 있는 것으로 보입니다.
// 저희는 이를 간소화 하여 UViewportClient를 만들었고, 여기에 직접적으로 명령을 전달할 예정입니다.
UCLASS()
class ENGINE_API UEditorViewportClient : public UViewportClient
{
	GENERATED_BODY()
public:
	UEditorViewportClient();
	~UEditorViewportClient();
	virtual void Init(HWND hInViewportHandle, UWorld* InWorld) override;
	virtual void Tick(float DeltaTime) override;
	virtual void Draw() override;

	/** 현재 뷰포트 타입에 대한 ViewportCameraTransform 객체를 가져옵니다. */
	FViewportCameraTransform& GetViewTransform()
	{
		return /*IsPerspective() ?*/ ViewTransformPerspective /* : ViewTransformOrthographic*/;
	}

	const FViewportCameraTransform& GetViewTransform() const
	{
		return /*IsPerspective() ? */ViewTransformPerspective /*: ViewTransformOrthographic*/;
	}

	/** @return 현재 뷰포트 카메라의 위치를 반환합니다. */
	const FVector& GetViewLocation() const
	{
		const FViewportCameraTransform& ViewTransform = GetViewTransform();
		return ViewTransform.GetLocation();
	}

	/** @return 현재 뷰포트 카메라의 회전을 반환합니다. */
	const FRotator& GetViewRotation() const
	{
		const FViewportCameraTransform& ViewTransform = GetViewTransform();
		return ViewTransform.GetRotation();
	}

	/** 뷰포트 카메라의 위치를 설정합니다 */
	void SetViewLocation(const FVector& NewLocation)
	{
		FViewportCameraTransform& ViewTransform = GetViewTransform();
		ViewTransform.SetLocation(NewLocation);
	}

	/** 뷰포트 카메라의 회전을 설정합니다 */
	void SetViewRotation(const FRotator& NewRotation)
	{
		FViewportCameraTransform& ViewTransform = GetViewTransform();
		ViewTransform.SetRotation(NewRotation);
	}

protected:
	/**
	 * 실시간 카메라 움직임을 업데이트합니다. 뷰포트의 각 틱마다 호출되어야 합니다!
	 *
	 * @param DeltaTime 마지막 업데이트 이후 경과된 시간(초)
	 */
	void UpdateCameraMovement(float DeltaTime);

	/**
	 * 델타 마우스 움직임을 기반으로 뷰포트를 업데이트하기 위해 매 프레임마다 호출됩니다.
	 */
	virtual void UpdateMouseDelta();

	/**
	 * 지정된 FSceneView 객체를 이 뷰포트의 뷰 및 투영 행렬로 구성합니다.
	 * @param	View		구성할 뷰입니다. 유효해야 합니다.
	 * @param	StereoPass	스테레오 모드에서 이 뷰를 그릴 때 어느 눈을 위한 것인지 지정합니다.
	 * @return	뷰포트의 기본 뷰를 나타내는 뷰 패밀리 내의 뷰에 대한 포인터를 반환합니다.
	 */
	virtual /*FSceneView**/void CalcSceneView(FSceneViewFamily* ViewFamily/*, const int32 StereoViewIndex = INDEX_NONE*/);

protected:
	/** 현재 캐시된 임펄스 상태 */
	FCameraControllerUserImpulseData* CameraUserImpulseData = nullptr;

	FEditorCameraController* CameraController = nullptr;

	/** 퍼스펙티브 뷰포트를 위한 뷰포트 카메라 변환 데이터 */
	FViewportCameraTransform ViewTransformPerspective;

private:
	DirectX::Mouse::State LastMouseState;
	DirectX::Mouse::State RightButtonStartMouseState; // 우클릭 시작한 시점의 마우스 상태
};