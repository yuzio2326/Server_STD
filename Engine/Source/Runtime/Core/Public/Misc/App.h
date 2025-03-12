#pragma once
#include "CoreGlobals.h"
#include "CoreTypes.h"
#undef GetCurrentTime
/**
 * 애플리케이션에 대한 정보를 제공합니다.
 */
class FApp
{
public:
	/** LastTime을 CurrentTime으로 업데이트합니다. */
	static void UpdateLastTime()
	{
		LastTime = CurrentTime;
	}

	/**
	 * 델타 시간을 초 단위로 가져옵니다.
	 *
	 * @return 델타 시간(초).
	 */
	FORCEINLINE static double GetDeltaTime()
	{
		return DeltaTime;
	}

	/**
	 * 현재 시간을 초 단위로 설정합니다.
	 *
	 * @param seconds - 초 단위 시간.
	 */
	static void SetCurrentTime(double Seconds)
	{
		CurrentTime = Seconds;
	}

	/**
	 * 현재 시간을 초 단위로 가져옵니다.
	 *
	 * @return 현재 시간(초).
	 */
	FORCEINLINE static double GetCurrentTime()
	{
		return CurrentTime;
	}

	/**
	 * 델타 시간을 초 단위로 설정합니다.
	 *
	 * @param seconds 초 단위 시간.
	 */
	static void SetDeltaTime(double Seconds)
	{
		DeltaTime = Seconds;
	}

	static uint32 GetFPS()
	{
		return FPS;
	}

	static void SetFPS(uint32 InFPS)
	{
		FPS = InFPS;
	}

private:
	/** 현재 시간을 보유합니다. */
	static inline CORE_API double CurrentTime = 0.0;

	/** CurrentTime의 이전 값을 보유합니다. */
	static inline CORE_API double LastTime = 0.0;

	/** 현재 델타 시간을 초 단위로 보유합니다. */
	static inline CORE_API double DeltaTime = 1.0 / 30.0;

	static inline CORE_API uint32 FPS = 0;
};