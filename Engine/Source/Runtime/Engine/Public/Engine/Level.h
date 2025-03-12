#pragma once
#include "CoreMinimal.h"
#include "Level.generated.h"

class FRegisterComponentContext;

//
// 레벨 객체. 레벨의 액터 목록, BSP 정보 및 브러시 목록을 포함합니다.
// 모든 레벨은 World를 Outer로 가지고 있으며 PersistentLevel로 사용할 수 있습니다.
// 그러나 레벨이 스트리밍되었을 때 OwningWorld는 그 레벨이 속한 World를 나타냅니다.
//

/**
 * 레벨은 액터(조명, 볼륨, 메쉬 인스턴스 등)의 모음입니다.
 * 여러 레벨을 월드에 로드하고 언로드하여 스트리밍 경험을 만들 수 있습니다.
 *
 * @see https://docs.unrealengine.com/latest/INT/Engine/Levels
 * @see UActor
 */
UCLASS()
class ENGINE_API ULevel : public UObject
{
	friend class AGameStateBase;
	GENERATED_BODY()
public:
	ULevel();

	virtual void Serialize(FArchive& Ar) override;

	/**
	 * 이 레벨과 관련된 모든 액터의 컴포넌트를 업데이트하고 BSP 모델 컴포넌트를 생성합니다.
	 * @param bRerunConstructionScripts 레벨의 액터에 대해 콜드 데이터를 다시 실행하려면 사용합니다.
	 */
	void UpdateLevelComponents(FRegisterComponentContext* Context = nullptr);

	/**
	 * 점진적으로 이 레벨과 관련된 모든 액터의 컴포넌트를 업데이트합니다.
	 *
	 * @param NumComponentsToUpdate     이 실행에서 업데이트할 컴포넌트 수, 모든 것을 업데이트하려면 0
	 * @param bRerunConstructionScripts 레벨의 액터에 대해 콜드 데이터를 다시 실행하려면 사용합니다.
	 */
	void IncrementalUpdateComponents(int32 NumComponentsToUpdate, FRegisterComponentContext* Context = nullptr);

	bool IncrementalRegisterComponents(bool bPreRegisterComponents, int32 NumComponentsToUpdate, FRegisterComponentContext* Context);

	/**
	 * 액터에게 사전 초기화 및 후 초기화를 라우팅하고 볼륨을 설정합니다.
	 *
	 * @param NumActorsToProcess    이 실행에서 업데이트할 액터 수의 최대값, 모든 액터를 처리하려면 0을 사용합니다.
	 * @todo seamless worlds: 이는 다중 레벨의 경우 볼륨을 올바르게 처리하지 않습니다.
	 */
	void RouteActorInitialize(int32 NumActorsToProcess);

public:
	/** 이 레벨의 모든 액터 배열 */
	TArray<TObjectPtr<AActor>> Actors;

	/**
	 * 이 레벨이 포함된 월드를 반환합니다.
	 * 스트리밍 레벨의 경우 GetOuter()는 사용되지 않는 잔재 월드이므로 GetOuter()와는 다릅니다.
	 * GC가 임의의 순서로 발생할 수 있으므로, BeginDestroy() 중에는 다른 UObject 참조와 마찬가지로 접근해서는 안 됩니다.
	 */
	//UPROPERTY(Transient)
	TEnginePtr<UWorld> OwningWorld;

public:
	enum class EIncrementalComponentState : uint8
	{
		Init,
		RegisterInitialComponents,
#if WITH_EDITOR
		RunConstructionScripts,
#endif
		Finalize
	};
	/** 레벨에서 액터 컴포넌트를 점진적으로 업데이트하는 데 사용되는 현재 단계 */
	EIncrementalComponentState IncrementalComponentState = EIncrementalComponentState::Init;

	/** 레벨의 액터 배열에서 컴포넌트를 업데이트하는 데 사용되는 현재 인덱스입니다. */
	int32 CurrentActorIndexForIncrementalUpdate = 0;

	/** CurrentActorIndexForUpdateComponents가 가리키는 액터가 PreRegisterAllComponents를 호출했는지 여부 */
	uint8 bHasCurrentActorCalledPreRegister : 1 = false;

	/** 컴포넌트가 현재 등록되어 있는지 여부. */
	uint8 bAreComponentsCurrentlyRegistered : 1 = false;

private:
	enum class ERouteActorInitializationState : uint8
	{
		Preinitialize,
		Initialize,
		BeginPlay,
		Finished
	};
	ERouteActorInitializationState RouteActorInitializationState = ERouteActorInitializationState::Preinitialize;
	int32 RouteActorInitializationIndex = 0;
};