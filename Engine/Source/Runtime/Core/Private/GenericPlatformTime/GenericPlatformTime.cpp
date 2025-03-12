#include "GenericPlatform/GenericPlatformTime.h"

CORE_API double FGenericPlatformTime::InitTiming()
{
	LARGE_INTEGER Frequency;
	bool bResult = QueryPerformanceFrequency(&Frequency);
	_ASSERT(bResult);
	SecondsPerCycle = 1.0 / (double)Frequency.QuadPart;
	//SecondsPerCycle64 = 1.0 / (double)Frequency.QuadPart;

	// Due to some limitation of the OS, we limit the polling frequency to 4 times per second, 
	// but it should be enough for longterm CPU usage monitoring.
	//static const float PollingInterval = 1.0f / 4.0f;

	//// Register a ticker delegate for updating the CPU utilization data.
	//FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateStatic(&FPlatformTime::UpdateCPUTime), PollingInterval);

	return FPlatformTime::Seconds();
}
