#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class IRendererModule;

/** 엔진 모듈을 구현합니다. */
class FEngineModule : public FDefaultModuleImpl
{
public:

    // IModuleInterface
    virtual void StartupModule();
    virtual void ShutdownModule();

protected:
    std::unique_ptr<DirectX::Keyboard> Keyboard;
    std::unique_ptr<DirectX::Mouse> Mouse;
};

/** 렌더러 모듈을 가져오고 결과를 캐시하는 접근자입니다. */
extern ENGINE_API IRendererModule& GetRendererModule();
