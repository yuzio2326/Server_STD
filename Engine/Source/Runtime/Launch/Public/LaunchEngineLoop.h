#pragma once
#include "CoreMinimal.h"

/**
 * 메인 엔진 루프를 구현합니다.
 */
class FEngineLoop
{
public:
    virtual ~FEngineLoop();
    /**
     * 메인 루프를 사전 초기화합니다 - 명령줄을 해석하고, GIsEditor 등을 설정합니다.
     *
     * @param CmdLine 명령줄 인자입니다.
     * @return 에러 레벨을 반환합니다; 성공 시 0, 에러 발생 시 > 0을 반환합니다.
     */
	int32 PreInit(const TCHAR* CmdLine);

    /**
     * 메인 루프를 초기화합니다 (나머지 초기화 과정).
     *
     * @return 에러 레벨을 반환합니다; 성공 시 0, 에러 발생 시 > 0을 반환합니다.
     */
    int32 Init(HWND hViewportWnd);

    /** 메인 루프를 진행합니다. */
    void Tick();

    void Exit();

    void WndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);

};