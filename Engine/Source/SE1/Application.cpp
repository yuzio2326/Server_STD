#include "CoreTypes.h"
#include "Modules/ModuleManager.h"
#include <iostream>

IMPLEMENT_MODULE(FDefaultModuleImpl, SE1);

extern int32 LAUNCH_API GuardedMain(const TCHAR* CmdLine);

#if WITH_EDITOR
#include "Application.h"
#include "Resource/Resource.h"
#include "MainFrame/MainFrm.h"
#include "Document/MFCApplicationDoc.h"
#include "View/MFCApplicationView.h"
#include "CoreMinimal.h"
//#include "Engine/World.h"
//#include "GameFramework/Actor.h"

void LAUNCH_API EngineTick();
void LAUNCH_API EngineExit();

BEGIN_MESSAGE_MAP(CMFCApplication, CWinAppEx)
	// 표준 파일을 기초로 하는 문서 명령입니다.
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
END_MESSAGE_MAP()

CMFCApplication theApp;

BOOL CMFCApplication::InitInstance()
{
	CWinAppEx::InitInstance();
	EnableTaskbarInteraction(FALSE);

	// RichEdit 컨트롤을 사용하려면 AfxInitRichEdit2()가 있어야 합니다.
	AfxInitRichEdit2();

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("로컬 애플리케이션 마법사에서 생성된 애플리케이션"));
	LoadStdProfileSettings(4);  // MRU를 포함하여 표준 INI 파일 옵션을 로드합니다.

	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// 애플리케이션의 문서 템플릿을 등록합니다.  문서 템플릿은
	//  문서, 프레임 창 및 뷰 사이의 연결 역할을 합니다.
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CMFCApplicationDoc),
		RUNTIME_CLASS(CMainFrame),       // 주 SDI 프레임 창입니다.
		RUNTIME_CLASS(CMFCApplicationView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// 표준 셸 명령, DDE, 파일 열기에 대한 명령줄을 구문 분석합니다.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// DDE Execute 열기를 활성화합니다.
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// 명령줄에 지정된 명령을 디스패치합니다.
	// 응용 프로그램이 /RegServer, /Register, /Unregserver 또는 /Unregister로 시작된 경우 FALSE를 반환합니다.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// 창 하나만 초기화되었으므로 이를 표시하고 업데이트합니다.
	m_pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);
	m_pMainWnd->UpdateWindow();
	// 접미사가 있을 경우에만 DragAcceptFiles를 호출합니다.
	//  SDI 응용 프로그램에서는 ProcessShellCommand 후에 이러한 호출이 발생해야 합니다.
	// 끌어서 놓기에 대한 열기를 활성화합니다.
	m_pMainWnd->DragAcceptFiles();

	return TRUE;
}

void CMFCApplication::OnWorldInitialized(UWorld* NewWorld)
{
	//CMainFrame* MainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
	//MainFrame->m_wndWorldOutliner.DeleteAllItems();
	//MainFrame->m_wndWorldOutliner.InsertRootItem(NewWorld->GetName().c_str());
	//MainFrame->m_wndWorldOutliner.Invalidate();
	//MainFrame->m_wndWorldOutliner.UpdateWindow();
}

void CMFCApplication::OnRefreshActors(vector<shared_ptr<AActor>>& NewActors)
{
	/*OnActorSelected_Details(nullptr);
	CMainFrame* MainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
	MainFrame->m_wndWorldOutliner.DeleteAllItems();
	MainFrame->m_wndWorldOutliner.InsertRootItem(GEngine->GetWorld()->GetName().c_str());

	for (shared_ptr<AActor>& It : NewActors)
	{
		OnActorSpawned(It.get());
	}*/
}

void CMFCApplication::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
}
void CMFCApplication::LoadCustomState()
{
}
void CMFCApplication::SaveCustomState()
{
}
BOOL CMFCApplication::OnIdle(LONG lCount)
{
	CWinAppEx::OnIdle(lCount);
	bool bExit = IsEngineExitRequested();
	if (bExit)
	{
		AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_APP_EXIT, 0);
		return FALSE;
	}

	EngineTick();
	////if (LastSelectedActor)
	////{
	////	OnActorSelected_Details(LastSelectedActor);
	////}

	return TRUE;
}
void CMFCApplication::OnClosingMainFrame(CFrameImpl* pFrameImpl)
{
	CWinAppEx::OnClosingMainFrame(pFrameImpl);
	EngineExit();
}
void CMFCApplication::OnAppAbout()
{
}
#elif CLIENT

#include <windows.h>
void LAUNCH_API EngineWndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	EngineWndProc(uMsg, wParam, lParam, nullptr);
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int32 LAUNCH_API EnginePreInit(const TCHAR* CmdLine);
int32 LAUNCH_API EngineInit(HWND hViewportWnd = NULL);

void LAUNCH_API EngineTick();
void LAUNCH_API EngineExit();

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WindowProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, TEXT("GameWindow"), NULL };
	RegisterClassEx(&wc);
	HWND hwnd = CreateWindow(wc.lpszClassName, TEXT("Simple Game Framework"), WS_OVERLAPPEDWINDOW, 100, 100, 800, 600, NULL, NULL, wc.hInstance, NULL);

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	int32 ErrorLevel = 0;
	ErrorLevel = EnginePreInit(nullptr);
	if (ErrorLevel != 0)
	{
		_ASSERT(false);
		ExitProcess(0);
	}

	ErrorLevel = EngineInit(hwnd);
	if (ErrorLevel != 0)
	{
		_ASSERT(false);
		ExitProcess(0);
	}

	MSG msg;
	bool running = true;
	while (running)
	{
		// 비활성 상태에서는 PeekMessage를 사용하여 메시지 큐를 처리
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				running = false;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// 여기서 게임 업데이트 및 렌더링 작업 수행
		EngineTick();
	}

	EngineExit();

	UnregisterClass(wc.lpszClassName, wc.hInstance);
	return (int)msg.wParam;
}


#else
int main()
{
	return GuardedMain(nullptr);
}
#endif