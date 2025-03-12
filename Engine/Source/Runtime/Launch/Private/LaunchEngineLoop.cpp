#include "LaunchEngineLoop.h"
#include "EngineModule.h"
#include "Engine/AssetManager.h"
#include "Engine/Engine.h"
#include "ShaderCompiler.h"
#include "RHI.h"

CORE_API map<FString, UClass*>& GetClassMap();

FEngineLoop::~FEngineLoop()
{
	FModuleManager::Get(true);
}

int32 FEngineLoop::PreInit(const TCHAR* CmdLine)
{
	FLogger::Get();
	FModuleManager::Get();
	FAssetManager::Get();
	FConfigCacheIni::Get();

	// PreInitPreStartupScreen
	{
		// LoadPreInitModules
		{
			FModuleManager::Get()->LoadModule(TEXT("Engine"));

#if !SERVER
			FModuleManager::Get()->LoadModule(TEXT("Renderer"));

			// LoadPreInitModules end

			RHIInit();
			{
				// Shader를 컴파일 한다
				FShaderCompilingManager::Get();

				{
					// 렌더링 모듈을 메인 스레드에 캐시하여 나중에 렌더링 스레드에서 안전하게 가져올 수 있도록 합니다.
					// * 우리는 렌더 스레드는 없음. 위 설명은 언리얼 설명
					GetRendererModule();
				}
			}
#endif
		}

	}

#if WITH_EDITOR
#ifdef USER_PROJECT_NAME
	{
		const FString UserProjectName = TEXT(USER_PROJECT_NAME);
		if (!UserProjectName.empty())
		{
			FModuleManager::Get()->LoadModule(FName(UserProjectName));
		}
	}
#endif
#endif

	// CDO 객체를 생성한다
	for (auto It : GetClassMap())
	{
		It.second->GetDefaultObject();
	}

	return 0;
}

int32 FEngineLoop::Init(HWND hViewportWnd)
{
	GEngine = NewObject<UEngine>(nullptr);
	GEngine->Init(hViewportWnd);

	return 0;
}

void FEngineLoop::Tick()
{
	GEngine->UpdateTimeAndHandleMaxTickRate();

	GEngine->Tick(FApp::GetDeltaTime());
}

void FEngineLoop::Exit()
{
	GEngine->PreExit();
	RHIExit();

	FConfigCacheIni::Get(true);
	FAssetManager::Get(true);
	GEngine = nullptr;

	// Unregister Class
	{
		for (auto It : GetClassMap())
		{
			It.second->~UClass();
			GetObjectArray().Free(typeid(UClass), It.second);
			It.second = nullptr;
		}
		GetClassMap().clear();
	}

	{
		GetObjectArray().Destroy();
	}

	FLogger::Get(true);
}

void FEngineLoop::WndProc(UINT Message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if (GEngine)
	{
		GEngine->WndProc(Message, wParam, lParam, pResult);
	}
}
