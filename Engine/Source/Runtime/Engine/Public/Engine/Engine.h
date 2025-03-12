#pragma once
#include "CoreMinimal.h"
#include "Engine/EngineBaseTypes.h"
#include "Engine/World.h"
#include "Engine.generated.h"

class UEditorViewportClient;
class UGameViewportClient;
class UViewportClient;
class UGameInstance;

UCLASS()
class ENGINE_API UEngine : public UObject
{
	friend class UGameInstance;
	GENERATED_BODY()

public:
	bool IsPIE() const { return bPIE; }

	UEngine();
	void Init(HWND hViewportHandle);
	void Tick(float DeltaSeconds);
	void PreExit();

	/**
	 * 최대 틱 속도를 고려하여 FApp::CurrentTime / FApp::DeltaTime을 업데이트합니다.
	 */
	void UpdateTimeAndHandleMaxTickRate();

public:
	string Save();
	void Load(const string& InLoadString);

public:
	void WndProc(UINT Message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);

public:
	virtual UWorld* GetEditorWorld() { return EditorWorld.get(); }
	virtual TObjectPtr<UWorld> CreatePIEWorldByDuplication(UWorld* InWorld);
	virtual void PostCreatePIEWorld(UWorld* InWorld);

	UViewportClient* GetCurrentViewClient() { return CurrentViewportClient.Get(); }

protected:
	/** 틱 레이트 제한기를 가져옵니다. */
	double GetMaxTickRate(double DeltaTime);

	/**
	 * 새로운 Play in Editor 인스턴스를 생성합니다 (하나의 프로세스에서 실행되지 않는 경우 새로운 프로세스에서 실행될 수 있습니다).
	 * 현재 세션 상태를 읽어 필요한 다음 인스턴스를 시작합니다.
	 * @param   InRequestParams         - 주로 글로벌 설정에 사용됩니다. 인스턴스별로 다르게 하지 말아야 합니다.
	 * @param   bInDedicatedInstance    - true인 경우, 전용 서버 인스턴스를 생성합니다. 여러 프로세스를 사용하는 경우, 사용하지 말아야 합니다.
	 * @param   InNetMode               - 이 인스턴스를 시작할 때 사용되는 네트 모드입니다. ListenServer와 같은 일부 네트 모드는 다음 인스턴스에 다른 네트 모드를 필요로 하기 때문에, RequestParams에서 가져오지 않습니다.
	 */
	virtual void CreateNewPlayInEditorInstance();

	// PIE에서 SIE로 전환
	void PIEtoSIE();

protected:
	bool bPIE = false;
	HWND MainViewportHandle = NULL;
	TObjectPtr<UWorld> PlayWorld;
	TObjectPtr<UWorld> EditorWorld;

	TObjectPtr<UEditorViewportClient> EditorViewportClient;

	/** 현재 게임 인스턴스를 나타내는 뷰포트입니다. null 일 수 있으므로 확인 없이 사용하지 마세요. */
	TObjectPtr<class UGameViewportClient> GameViewport;

	TEnginePtr<UViewportClient> CurrentViewportClient;

protected: // PIE
	TObjectPtr<UGameInstance> GameInstance;
};

extern ENGINE_API UWorld* GWorld;

/** Global engine pointer. Can be 0 so don't use without checking. */
extern ENGINE_API TObjectPtr<UEngine> GEngine;