#pragma once
#include "Actor.h"
#include "GameModeBase.generated.h"

class UPlayer;
class AController;
class APlayerController;
class AGameSession;
class AGameStateBase;
class APlayerState;

UCLASS()
class ENGINE_API AGameModeBase : public AActor
{
	GENERATED_BODY()
public:
	AGameModeBase();
	virtual void PreInitializeComponents() override;

public:

	//~=============================================================================
	// 게임 초기화

	/**
	 * 게임을 초기화합니다.
	 * GameMode의 InitGame() 이벤트는 다른 함수들(PreInitializeComponents() 포함)보다 먼저 호출되며,
	 * GameMode가 매개변수를 초기화하고 헬퍼 클래스를 스폰하는 데 사용됩니다.
	 * @경고: 이는 액터의 PreInitializeComponents보다 먼저 호출됩니다.
	 */
	virtual void InitGame(/*const FString& MapName, const FString& Options, FString& ErrorMessage*/);


	/**
	 * 새 플레이어를 로그인하여 플레이어 컨트롤러를 생성하고, 게임에서 재정의할 수 있습니다.
	 *
	 * 플레이어의 기본 속성(이름, 고유 ID, 백엔드 등록 등)을 설정하며, 더 복잡한 게임 로직을 수행하는 데 사용해서는 안 됩니다.
	 * 네트워킹과 관련하여 이 함수 내에서 플레이어 컨트롤러는 완전히 초기화되지 않습니다.
	 * "게임 로직"은 바로 뒤에 호출되는 PostLogin에 저장해야 합니다.
	 *
	 * @param NewPlayer 이 플레이어를 나타내는 UPlayer 객체 포인터(로컬 또는 원격).
	 * @param RemoteRole 이 컨트롤러가 가지는 원격 역할.
	 * @param Portal 클라이언트가 지정한 원하는 포탈 위치.
	 * @param Options 로그인 시 클라이언트가 전달한 게임 옵션.
	 * @param UniqueId 로그인하는 플레이어에 대한 플랫폼 고유 식별자.
	 * @param ErrorMessage [out] 오류 메시지, 있는 경우, 왜 이 로그인이 실패하는지 설명합니다.
	 *
	 * 로그인이 성공하면, 이 플레이어와 연관된 새 PlayerController를 반환합니다. 오류 메시지 문자열이 설정된 경우 로그인은 실패합니다.
	 *
	 * @return 로그인한 플레이어의 새 플레이어 컨트롤러, 어떤 이유로 로그인이 실패하면 NULL.
	 */
	virtual APlayerController* Login(UPlayer* NewPlayer/*, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage*/);

	/** 로그인 성공 후 호출됩니다. PlayerController에서 복제된 함수를 호출할 수 있는 첫 번째 안전한 장소입니다. */
	virtual void PostLogin(APlayerController* NewPlayer);

	/** PostLogin 프로세스의 일부로 호출됩니다. 새로운 플레이어를 생성하기 전의 마지막 단계입니다. */
	void DispatchPostLogin(AController* NewPlayer);

	/** 플레이어가 게임에 참여할 준비가 되었음을 알리며, 이는 게임을 시작할 수 있습니다. */
	//UFUNCTION(BlueprintNativeEvent, Category = Game)
	void HandleStartingNewPlayer(APlayerController* NewPlayer);

	/** FindPlayerStart가 반환한 위치에서 플레이어의 폰을 생성하려고 시도합니다. */
	//UFUNCTION(BlueprintCallable, Category = Game)
	virtual void RestartPlayer(AController* NewPlayer);

	/** 지정된 액터의 위치에서 플레이어의 폰을 생성하려고 시도합니다. */
	//UFUNCTION(BlueprintCallable, Category = Game)
	virtual void RestartPlayerAtPlayerStart(AController* NewPlayer, AActor* StartSpot);

	/** RestartPlayer의 두 번째 절반을 처리합니다. */
	virtual void FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation);


	//~=============================================================================
	
	/**
	 * 주어진 옵션에 대해 적절한 PlayerController를 생성합니다; 더 쉽게 재정의할 수 있도록 Login()에서 분리되었습니다.
	 * 예를 들어, 조건에 따라 특화된 PlayerController를 생성하려면 이를 재정의하세요.
	 *
	 * @param RemoteRole 이 컨트롤러가 원격으로 수행할 역할.
	 * @param Options 새 플레이어의 URL에서 가져온 옵션 문자열.
	 *
	 * @return 플레이어에 대한 PlayerController, 이 플레이어가 존재해서는 안 되는 이유가 있거나 오류가 있는 경우 NULL.
	 */
	virtual APlayerController* SpawnPlayerController(/*ENetRole InRemoteRole, const FString& Options*/);

	//~=============================================================================
	// 게임 시작/일시정지/초기화

	/** 액터에서 BeginPlay를 호출하도록 전환합니다. */
	//UFUNCTION(BlueprintCallable, Category = Game)
	virtual void StartPlay();

protected:
	/** Called as part of DispatchPostLogin */
	virtual void OnPostLogin(AController* NewPlayer) {}

protected:
	/** 게임 세션은 로그인 승인, 중재, 온라인 게임 인터페이스를 처리합니다. */
	//UPROPERTY(Transient)
	AGameSession* GameSession = nullptr;

	/** GameState는 모든 클라이언트에 관련 속성을 복제하는 데 사용됩니다. */
	//UPROPERTY(Transient)
	AGameStateBase* GameState = nullptr;

public:
	/** 로그인하는 플레이어를 위해 스폰할 PlayerController의 클래스입니다. */
	//UPROPERTY(EditAnywhere, NoClear, BlueprintReadOnly, Category = Classes)
	TSubclassOf<APlayerController> PlayerControllerClass;

	/** 이 GameMode와 연관된 GameState의 클래스입니다. */
	//UPROPERTY(EditAnywhere, NoClear, BlueprintReadOnly, Category = Classes)
	TSubclassOf<AGameStateBase> GameStateClass;

	/** 이 클래스의 PlayerState는 모든 플레이어와 연관되어 플레이어의 관련 정보를 모든 클라이언트에 복제합니다. */
	//UPROPERTY(EditAnywhere, NoClear, BlueprintReadOnly, Category = Classes)
	TSubclassOf<APlayerState> PlayerStateClass;

	TSubclassOf<APawn> DefaultPawnClass;
};