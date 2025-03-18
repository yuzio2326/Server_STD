#pragma once
#include "CoreTypes.h"
#include "Containers/JobQue.h"
#include "Delegate/Delegate.h"

//namespace ELogVerbosity
//{
	enum ELogVerbosity : uint8
	{
		/** Always prints a fatal error to console (and log file) and crashes (even if logging is disabled) */
		Fatal,

		/**
		 * Prints an error to console (and log file).
		 * Commandlets and the editor collect and report errors. Error messages result in commandlet failure.
		 */
		Error,

		/**
		 * Prints a warning to console (and log file).
		 * Commandlets and the editor collect and report warnings. Warnings can be treated as an error.
		 */
		Warning,

		/** Prints a message to a log file (does not print to console) */
		Log,
	};
//}
inline const TCHAR* GetLogName(ELogVerbosity InLogLevel)
{
	switch (InLogLevel)
	{
	case Log:	 return TEXT("Log");
	case Warning: return TEXT("Warning");
	case Error: return TEXT("Error");
	case Fatal: return TEXT("Fatal");
	}
	_ASSERT(false);
	return nullptr;
}

// CUSTOM_LOG_THREAD 가 1이면 스레드 학습 용 Log Thread를 활성화 합니다
#define CUSTOM_LOG_THREAD 1


class CORE_API FLogger
{
public:
	static FLogger* Get(const bool bDestroy = false);
	
	FLogger();
	void LogF(ELogVerbosity InLogVerbosity, FStringView InMessage);

	FDelegate<ELogVerbosity, FStringView> LogDelegate;

#if CUSTOM_LOG_THREAD
private:
	jthread LogThread;
	FJobQueue JobQueue;
#endif

};

#define E_LOG(LogVerbosity, FormatMsg, ...) FLogger::Get()->LogF(ELogVerbosity::LogVerbosity, format(FormatMsg, __VA_ARGS__));