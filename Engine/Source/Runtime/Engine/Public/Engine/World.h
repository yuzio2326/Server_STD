#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/EngineTypes.h"
#include "World.generated.h"

class ULevel;
class APawn;
class UGameInstance;
class APlayerController;
class AGameModeBase;
class AGameStateBase;
class UGameViewportClient;
struct FURL;

struct ENGINE_API FActorSpawnParameters
{
	FActorSpawnParameters();

	/* 스폰된 액터의 이름으로 지정할 이름입니다. 값이 지정되지 않으면, 스폰된 액터의 이름은 [Class]_[Number] 형식으로 자동 생성됩니다. */
	FName Name;

	/* 새로운 액터를 스폰할 때 템플릿으로 사용할 액터입니다. 스폰된 액터는 템플릿 액터의 속성 값을 사용하여 초기화됩니다. 만약 이 값이 NULL로 남아있으면, 클래스 기본 객체 (CDO)를 사용하여 스폰된 액터를 초기화합니다. */
	AActor* Template;

	/* 이 액터를 스폰한 액터입니다. (NULL로 남겨둘 수 있습니다). */
	AActor* Owner;

	/* 스폰된 액터가 입힌 피해에 책임이 있는 APawn입니다. (NULL로 남겨둘 수 있습니다). */
	APawn* Instigator;

	/** 제공된 스폰 Transform을 사용하여 루트 컴포넌트를 곱할지 무시할지 결정합니다 */
	ESpawnActorScaleMethod TransformScaleMethod = ESpawnActorScaleMethod::MultiplyWithRoot;

	/* 스폰된 액터/객체 인스턴스를 설명하는 데 사용되는 플래그입니다. */
	EObjectFlags ObjectFlags;
};

/**
 * World는 액터와 컴포넌트가 존재하고 렌더링되는 맵 또는 샌드박스를 나타내는 최상위 객체입니다.
 *
 * World는 단일 Persistent Level과 볼륨 및 블루프린트 함수로 로드되고 언로드되는 선택적 스트리밍 레벨 목록을 포함할 수 있으며,
 * 또는 World Composition으로 구성된 레벨 모음일 수 있습니다.
 *
 * 독립 실행형 게임에서는 일반적으로 하나의 World만 존재합니다. 단, 매끄러운 영역 전환 동안에는 목적지와 현재 월드가 모두 존재할 수 있습니다.
 * 에디터에서는 여러 World가 존재합니다: 편집 중인 레벨, 각 PIE 인스턴스, 상호작용 렌더 뷰포트를 가진 각 에디터 도구 등 다양한 경우가 있습니다.
 *
 */
UCLASS()
class ENGINE_API UWorld : public UObject
{
	friend class AGameStateBase;
	friend class UGameInstance;
	friend class UEngine;
	GENERATED_BODY()
public:
	UWorld();
	~UWorld();

	/** Sets the owning game instance for this world */
	inline void SetGameInstance(UGameInstance* NewGI)
	{
		OwningGameInstance = NewGI;
	}

	/** Returns the owning game instance for this world */
	inline UGameInstance* GetGameInstance() const
	{
		return OwningGameInstance;
	}

	/**
	 * 현재 게임 모드 인스턴스를 반환합니다. 서버의 게임 플레이 도중에는 항상 유효합니다.
	 * 서버에서만 유효한 포인터를 반환합니다. 클라이언트에서는 항상 null을 반환합니다.
	 */
	AGameModeBase* GetAuthGameMode() const { return AuthorityGameMode; }

	/** 현재 GameState 인스턴스를 이 월드와 GameState의 레벨 컬렉션에 설정합니다. */
	void SetGameState(AGameStateBase* NewGameState);

public:
	/**
	 * 새로 생성된 월드를 초기화합니다.
	 */
	void InitalizeNewWorld();

	/**
	 * 모든 액터를 초기화하고 게임플레이를 시작할 준비를 합니다
	 * @param InURL 명령줄 URL
	 * @param bResetTime (선택 사항) WorldSettings의 TimeSeconds를 0으로 초기화할지 여부
	 */
	void InitializeActorsForPlay(/*const FURL& InURL, bool bResetTime = true,*/ FRegisterComponentContext* Context = nullptr);

	/**
	 * 선 그리기 배치기(line batcher) 및 모든 레벨 구성 요소와 같은 월드 구성 요소를 업데이트합니다.
	 *
	 * @param   bRerunConstructionScripts   액터의 구축 스크립트를 다시 실행할지 여부
	 * @param   bCurrentLevelOnly           참일 경우, 현재 레벨에만 영향을 미칩니다.
	 */
	void UpdateWorldComponents(/*bool bRerunConstructionScripts, *//*bool bCurrentLevelOnly,*/ FRegisterComponentContext* Context = nullptr);


	/**
	 * 월드를 초기화하고, 지속적인 레벨을 연결하며, 적절한 영역을 설정합니다.
	 */
	void InitWorld();

	void Tick(float DeltaSeconds);

public:
	/**
	 * 에디터 월드를 복제하여 PIE 월드를 생성합니다.
	 */
	static TObjectPtr<UWorld> GetDuplicatedWorldForPIE(UWorld* InWorld);

public:
	virtual void Serialize(FArchive& Ar) override;
	string Save();
	void Load(const string& InLoadString);

public:
	/**
	 * 주어진 Transform과 스폰 파라미터를 사용하여 액터를 스폰합니다.
	 *
	 * @param   Class                   스폰할 클래스
	 * @param   Transform               스폰할 월드 변환
	 * @param   SpawnParameters         스폰 파라미터
	 *
	 * @return  방금 스폰된 액터
	*/
	AActor* SpawnActor(UClass* Class, FTransform const* Transform, const FActorSpawnParameters& SpawnParameters = FActorSpawnParameters());

	/**
	 *  SpawnActor의 템플릿 버전입니다.
	 */
	template< class T >
	T* SpawnActor(UClass* Class, FTransform const& Transform, const FActorSpawnParameters& SpawnParameters = FActorSpawnParameters())
	{
		if (Class == nullptr)
		{
			Class = T::StaticClass();
		}
		return CastCheckedRaw<T>(SpawnActor(Class, &Transform, SpawnParameters));
	}

	/**
	 *  SpawnActor의 템플릿 버전입니다.
	 */
	template< class T >
	T* SpawnActor(UClass* Class = nullptr, const FActorSpawnParameters& SpawnParameters = FActorSpawnParameters())
	{
		if (Class == nullptr)
		{
			Class = T::StaticClass();
		}
		return SpawnActor<T>(Class, FTransform::Identity, SpawnParameters);
	}

	/**
	 * PlayerController를 생성하고 지정된 RemoteRole 및 옵션으로 전달된 Player에 바인딩합니다.
	 *
	 * @param Player - PlayerController에 설정할 플레이어입니다.
	 * @param RemoteRole - PlayerController에 설정할 RemoteRole입니다.
	 * @param URL - 플레이어 옵션(이름 등)을 포함하는 URL입니다.
	 * @param UniqueId - 플레이어의 고유 네트워크 ID입니다(온라인 서브시스템이 없거나 로그인하지 않은 경우, 예: 로컬 게임 또는 LAN 매치의 경우 0일 수 있습니다).
	 * @param Error (out) - 설정된 경우, 오류가 있음을 나타냅니다. 보통 호출 코드가 실제 메시지를 조회할 수 있는 속성으로 설정됩니다.
	 * @param InNetPlayerIndex (optional) - PlayerController에 설정할 NetPlayerIndex입니다.
	 * @return 생성된 PlayerController입니다(실패할 경우 NULL을 반환할 수 있습니다).
	 */
	//UE_DEPRECATED(5.0, "FUniqueNetIdRepl을 사용하는 SpawnPlayActor를 사용하세요.")
	//	APlayerController* SpawnPlayActor(class UPlayer* Player, ENetRole RemoteRole, const FURL& InURL, const FUniqueNetIdPtr& UniqueId, FString& Error, uint8 InNetPlayerIndex = 0);
	APlayerController* SpawnPlayActor(class UPlayer* Player/*, ENetRole RemoteRole, const FURL& InURL, const FUniqueNetIdRepl& UniqueId, FString& Error, uint8 InNetPlayerIndex = 0*/);

public:
	/** 액터가 초기화되고 게임을 시작할 준비가 되면 true를 반환합니다. */
	bool AreActorsInitialized() const;

public:
	/** 이 월드가 게임 월드(PIE 월드 포함)인지 여부를 반환합니다. */
	bool IsGameWorld() const;

	/** 이 월드가 에디터 월드(에디터 미리보기 월드 포함)인지 여부를 반환합니다. */
	bool IsEditorWorld() const;

	/** 이 월드의 타입입니다. 사용되는 Context을 설명합니다(에디터, 게임, 프리뷰 등). */
	EWorldType::Type WorldType = EWorldType::Type::None;

public:
	void OnWorldChanged();

	/** 액터에 대해 BeginPlay를 호출했는지 설정합니다. */
	void SetBegunPlay(bool bHasBegunPlay);

	/** 액터에 대해 BeginPlay가 호출되었는지 가져옵니다. */
	bool GetBegunPlay() const;

	/** 게임 플레이가 이미 시작되었는지 여부를 반환합니다. */
	bool HasBegunPlay() const;

public:
	/** 레벨의 GameMode를 생성합니다. */
	bool SetGameMode(const FURL& InURL);

	/**
	 * 게임 플레이를 시작합니다. 이는 게임 모드를 올바른 상태로 전환시키고, 모든 액터에서 BeginPlay를 호출합니다.
	 */
	void BeginPlay();

private:
	/** 서버에서만 유효한 현재 GameMode */
	//UPROPERTY(/*Transient*/)
	class AGameModeBase* AuthorityGameMode = nullptr;

	/** 클라이언트가 접근할 수 있는 게임 상태 정보를 포함하는 복제된 액터입니다. 직접 접근은 허용되지 않으며, GetGameState<>()를 사용해야 합니다. */
	//UPROPERTY(Transient)
	class AGameStateBase* GameState = nullptr;

	/** 월드 정보, 기본 브러시 및 게임 플레이 중 스폰된 액터 등을 포함하는 PersistentLevel */
	TObjectPtr<ULevel> PersistentLevel;

	//UPROPERTY(Transient)
	UGameInstance*	OwningGameInstance = nullptr;

	FTransform Transform = FTransform::Identity;

	UGameViewportClient* GameViewport = nullptr;

public:
	/**
	 * 액터가 스폰될 때마다 알림을 브로드캐스트합니다.
	 * 이 이벤트는 새로 생성된 액터에만 해당됩니다.
	 */
	FDelegate<AActor*> OnActorSpawned;

	// PIE에서 Actor들이 복제되고 호출됩니다
	FDelegate<UWorld*, TArray<TObjectPtr<AActor>>&> WorldChangedDelegate;

public:
	/** 이 월드의 씬 매니저에 대한 인터페이스. */
	class FSceneInterface* Scene = nullptr;

	/** 액터가 게임을 위해 초기화되었는지 여부 */
	uint8 bActorsInitialized : 1 = false;

	/** 액터에 대해 BeginPlay가 호출되었는지 여부 */
	uint8 bBegunPlay : 1 = false;
};

extern ENGINE_API FDelegate<UWorld*> WorldCreatedDelegate;
extern ENGINE_API FDelegate<UWorld*> WorldDestroyedDelegate;