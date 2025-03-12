
// MFCApplication1View.cpp: CMFCApplicationView 클래스의 구현
//
#if WITH_EDITOR
#include "MFCApplicationView.h"
#include "CoreTypes.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif


// CMFCApplicationView

IMPLEMENT_DYNCREATE(CMFCApplicationView, CView)

BEGIN_MESSAGE_MAP(CMFCApplicationView, CView)
END_MESSAGE_MAP()

// CMFCApplicationView 생성/소멸

CMFCApplicationView::CMFCApplicationView() noexcept
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

CMFCApplicationView::~CMFCApplicationView()
{
}

BOOL CMFCApplicationView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CMFCApplicationView 그리기

void CMFCApplicationView::OnDraw(CDC* pDC)
{
	/*CMFCApplicationDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;*/

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
	//CBrush redBrush(RGB(0, 255, 255));
	//CBrush* pOldBrush = pDC->SelectObject(&redBrush);

	//// 클라이언트 영역 크기 가져오기
	//CRect rect;
	//GetClientRect(&rect);

	//// 사각형 그리기
	//pDC->Rectangle(&rect);

	//// 원래 브러시로 복원
	//pDC->SelectObject(pOldBrush);
}

//void CMFCApplicationView::OnRButtonUp(UINT /* nFlags */, CPoint point)
//{
//	//ClientToScreen(&point);
//	//OnContextMenu(this, point);
//}
//
//void CMFCApplicationView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
//{
////#ifndef SHARED_HANDLERS
//	//theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
////#endif
//}

// CMFCApplicationView 진단

//#ifdef _DEBUG
//void CMFCApplicationView::AssertValid() const
//{
//	CView::AssertValid();
//}
//
//void CMFCApplicationView::Dump(CDumpContext& dc) const
//{
//	CView::Dump(dc);
//}
//
//
//CMFCApplicationDoc* CMFCApplicationView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
//{
//	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMFCApplicationDoc)));
//	return (CMFCApplicationDoc*)m_pDocument;
//}
//#endif //_DEBUG
int32 LAUNCH_API EnginePreInit(const TCHAR* CmdLine);
int32 LAUNCH_API EngineInit(HWND hViewportWnd = NULL);
bool bInit = false;
void CMFCApplicationView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	if (bInit == true) { return; }
	bInit = true;
	int32 ErrorLevel = 0;
	ErrorLevel = EnginePreInit(nullptr);
	if (ErrorLevel != 0) 
	{
		_ASSERT(false);
		ExitProcess(0);
	}

	ErrorLevel = EngineInit(m_hWnd);
	if (ErrorLevel != 0)
	{
		_ASSERT(false);
		ExitProcess(0);
	}
}

void LAUNCH_API EngineWndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
BOOL CMFCApplicationView::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	EngineWndProc(message, wParam, lParam, pResult);

	return CView::OnWndMsg(message, wParam, lParam, pResult);
}

#endif // #if WITH_EDITOR