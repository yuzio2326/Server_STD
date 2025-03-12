#pragma once
#include "CoreMinimal.h"
#include "SceneInterface.h"
#include "RenderGraphUtils.h"

class FSceneViewFamily;
class FRenderTarget;

/**
 * 장면 공간에서 2D 화면 영역으로의 투영.
 */
//class FSceneView
//{
//public:
//	const FSceneViewFamily* Family;
//
//};

/**
 * 뷰 변환과 소유자 액터만 다른 장면에 대한 여러 뷰의 집합.
 */
class FSceneViewFamily
{
public:
	/**
	* FSceneViewFamily 인스턴스를 생성하는 헬퍼 구조체
	* 시간을 지정하여 생성하면 해당 장면에서 시간을 가져옵니다. (우리는 없음)
	*
	* @param InRenderTarget     뷰가 렌더링될 렌더 타겟.
	* @param InScene            조회되는 장면.
	* @param InShowFlags        뷰에 대한 표시 플래그.
	*
	*/
	struct ConstructionValues
	{
		ConstructionValues(ConstructionValues&&) = default;
		ConstructionValues(const ConstructionValues& Other) = default;
		ConstructionValues& operator=(ConstructionValues&&) = default;
		ConstructionValues& operator=(const ConstructionValues& Other) = default;

		ENGINE_API ConstructionValues(
			FRHIViewport* InRenderTarget, // FRenderTarget
			FSceneInterface* InScene//,
			//const FEngineShowFlags& InEngineShowFlags
		);

		/** 뷰 패밀리를 구성하는 뷰들. */
		FRHIViewport* RenderTarget; // FRenderTarget

		/** 뷰가 렌더링되는 렌더 타겟. */
		FSceneInterface* Scene;

		/** true인 경우 장면 렌더링 결과를 렌더 타겟에 복사/해결함. */
		uint32 bResolveScene : 1;

		/** true인 경우 장면 렌더링 결과를 렌더 타겟에 복사/해결할지 여부를 설정함. */
		ConstructionValues& SetResolveScene(const bool Value) { bResolveScene = Value; return *this; }
	};

	/** 뷰 패밀리를 구성하는 뷰들. */
	//TArray<const FSceneView*> Views;

	/** 뷰가 렌더링되는 렌더 타겟. */
	FRHIViewport* RenderTarget = nullptr;

	/** 조회되는 장면. */
	FSceneInterface* Scene = nullptr;

	/** true인 경우 장면 렌더링 결과를 렌더 타겟에 복사/해결함. */
	uint32 bResolveScene : 1;

	FVector3D EyePosition;
	Matrix ViewMatrix = Matrix::Identity;
	Matrix ViewRotationMatrix = Matrix::Identity;
	Matrix ProjectionMatrix = Matrix::Identity;
	Matrix ViewProjectionMatrix = Matrix::Identity;

	FVector2D ViewportSize = FVector2D::Zero;

	bool bIsSceneTexturesInitialized = false;
	bool bIsViewFamilyInfo = false;

	/** Initialization constructor. */
	ENGINE_API FSceneViewFamily(const ConstructionValues& CVS);
	ENGINE_API virtual ~FSceneViewFamily();
};

/**
 * 범위를 벗어날 때 뷰를 삭제하는 뷰 패밀리.
 */
class FSceneViewFamilyContext : public FSceneViewFamily
{
public:
    /** 초기화 생성자. */
    FSceneViewFamilyContext(const ConstructionValues& CVS)
        : FSceneViewFamily(CVS)
    {
    }

    /** 소멸자. */
    ENGINE_API virtual ~FSceneViewFamilyContext();
};
