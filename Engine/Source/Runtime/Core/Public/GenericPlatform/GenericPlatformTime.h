#pragma once
#include "CoreTypes.h"

struct FGenericPlatformTime
{
	static CORE_API double InitTiming();

	static FORCEINLINE double Seconds()
	{
		LARGE_INTEGER Cycles;
		QueryPerformanceCounter(&Cycles);

		// 큰 숫자를 더하여 반환 값이 float로 전달될 때 발생할 수 있는 버그를 명확히 하기 위함
		return (double)Cycles.QuadPart * GetSecondsPerCycle() + 16777216.0;
	}

	/**
	* @return seconds per cycle.
	*/
	static double GetSecondsPerCycle()
	{
		return SecondsPerCycle;
	}

protected:
	static inline CORE_API double SecondsPerCycle = 0;
};

typedef FGenericPlatformTime FPlatformTime;