#include "Launch.h"
#include "LaunchEngineLoop.h"

IMPLEMENT_MODULE(FDefaultModuleImpl, Launch);

FEngineLoop GEngineLoop;

/**
 * PreInits the engine loop
 */
int32 LAUNCH_API EnginePreInit(const TCHAR* CmdLine)
{
	int32 ErrorLevel = GEngineLoop.PreInit(CmdLine);

	return(ErrorLevel);
}

/**
 * Inits the engine loop
 */
int32 LAUNCH_API EngineInit(HWND hViewportWnd)
{
	int32 ErrorLevel = GEngineLoop.Init(hViewportWnd);

	return(ErrorLevel);
}

/**
 * Ticks the engine loop
 */
LAUNCH_API void EngineTick(void)
{
	GEngineLoop.Tick();
}

LAUNCH_API void EngineExit(void)
{
	GEngineLoop.Exit();
}

void LAUNCH_API EngineWndProc(UINT Message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	GEngineLoop.WndProc(Message, wParam, lParam, pResult);
}

int32 LAUNCH_API GuardedMain(const TCHAR* CmdLine)
{
	int32 ErrorLevel = EnginePreInit(CmdLine);
	if (ErrorLevel != 0) { return ErrorLevel; }

	ErrorLevel = EngineInit(NULL);
	if (ErrorLevel != 0) { return ErrorLevel; }
	
	while (!IsEngineExitRequested())
	{
		EngineTick();
	}

	EngineExit();

	return ErrorLevel;
}