#include "CoreTypes.h"
#include "Modules/ModuleManager.h"
#include "GenericPlatform/GenericPlatformTime.h"

class FCoreModule : public FDefaultModuleImpl
{
public:
	virtual bool SupportsDynamicReloading() override
	{
		// Core cannot be unloaded or reloaded
		return false;
	}
};

IMPLEMENT_MODULE(FCoreModule, Core);

/** FPlatformTime::Seconds()가 처음으로 초기화된 시간(메인 함수 이전) */
double GStartTime = FPlatformTime::InitTiming();