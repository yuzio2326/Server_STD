#pragma once
#include "CoreTypes.h"
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

class CORE_API FLogger
{
public:
	static FLogger* Get(const bool bDestroy = false);
	
	FLogger();
	void LogF(ELogVerbosity InLogVerbosity, FStringView InMessage);

	FDelegate<ELogVerbosity, FStringView> LogDelegate;
};

#define E_LOG(LogVerbosity, FormatMsg, ...) FLogger::Get()->LogF(ELogVerbosity::LogVerbosity, format(FormatMsg, __VA_ARGS__));