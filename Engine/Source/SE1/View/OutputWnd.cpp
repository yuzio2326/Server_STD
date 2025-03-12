#if WITH_EDITOR
#include "Logging/Logger.h"
#include "Misc/MFCHeaders.h"
#include "MainFrame/MainFrm.h"
#include "OutputWnd.h"
#include "Resource/Resource.h"
//
//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif

/////////////////////////////////////////////////////////////////////////////
// COutputBar

COutputWnd::COutputWnd() noexcept
{
}

COutputWnd::~COutputWnd()
{
}

void COutputWnd::OnLog(ELogVerbosity InLogVerbosity, FStringView InMessage)
{
	FString NewString = TEXT("[") + FString(GetLogName(InLogVerbosity)) + TEXT("] ") + InMessage.data();
	m_wndOutputDebug.AddString(NewString.data());

	// 로그 항목이 100개를 넘으면 가장 오래된 항목을 제거합니다.
	const int MaxLogItems = 100;
	int ItemCount = m_wndOutputDebug.GetCount();
	if (ItemCount > MaxLogItems)
	{
		m_wndOutputDebug.DeleteString(0); // 가장 오래된 항목 제거
	}

	// 스크롤을 가장 아래로 내린다
	ItemCount = m_wndOutputDebug.GetCount(); // ItemCount를 다시 계산합니다.
	if (ItemCount > 0)
	{
		m_wndOutputDebug.SetTopIndex(ItemCount - 1);
	}
}

BEGIN_MESSAGE_MAP(COutputWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

int COutputWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 탭 창을 만듭니다.
	if (!m_wndTabs.Create(CMFCTabCtrl::STYLE_FLAT, rectDummy, this, 1))
	{
		TRACE0("출력 탭 창을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	// 출력 창을 만듭니다.
	const DWORD dwStyle = LBS_NOINTEGRALHEIGHT | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL;

	/*if (!m_wndOutputBuild.Create(dwStyle, rectDummy, &m_wndTabs, 2) ||
		!m_wndOutputDebug.Create(dwStyle, rectDummy, &m_wndTabs, 3) ||
		!m_wndOutputFind.Create(dwStyle, rectDummy, &m_wndTabs, 4))*/
	if (!m_wndOutputDebug.Create(dwStyle, rectDummy, &m_wndTabs, 2))
	{
		TRACE0("출력 창을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}
	UpdateFonts();

	CString strTabName;
	BOOL bNameValid;

	// 탭에 목록 창을 연결합니다.
	//bNameValid = strTabName.LoadString(IDS_BUILD_TAB);
	//ASSERT(bNameValid);
	//m_wndTabs.AddTab(&m_wndOutputBuild, strTabName, (UINT)0);
	bNameValid = strTabName.LoadString(IDS_DEBUG_TAB);
	ASSERT(bNameValid);
	m_wndTabs.AddTab(&m_wndOutputDebug, strTabName, (UINT)0);
	//bNameValid = strTabName.LoadString(IDS_FIND_TAB);
	//ASSERT(bNameValid);
	//m_wndTabs.AddTab(&m_wndOutputFind, strTabName, (UINT)2);
	m_wndTabs.SetActiveTabColor(RGB(32, 32, 32));
	m_wndTabs.SetActiveTabTextColor(RGB(255, 255, 255));

	const int TabCount = 1;
	for (int i = 0; i < TabCount; ++i)
	{
		m_wndTabs.SetTabBkColor(i, RGB(32, 32, 32));
		m_wndTabs.SetTabTextColor(i, RGB(125, 125, 125));
	}

	// 출력 탭을 더미 텍스트로 채웁니다.
	//FillBuildWindow();
	//FillDebugWindow();
	//FillFindWindow();

	FLogger::Get()->LogDelegate.AddRow(this, &COutputWnd::OnLog);
	return 0;
}

void COutputWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// Tab 컨트롤은 전체 클라이언트 영역을 처리해야 합니다.
	m_wndTabs.SetWindowPos (nullptr, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
}

void COutputWnd::AdjustHorzScroll(CListBox& wndListBox)
{
	CClientDC dc(this);
	CFont* pOldFont = dc.SelectObject(&afxGlobalData.fontRegular);

	int cxExtentMax = 0;

	for (int i = 0; i < wndListBox.GetCount(); i ++)
	{
		CString strItem;
		wndListBox.GetText(i, strItem);

		cxExtentMax = max(cxExtentMax, (int)dc.GetTextExtent(strItem).cx);
	}

	wndListBox.SetHorizontalExtent(cxExtentMax);
	dc.SelectObject(pOldFont);
}

void COutputWnd::FillBuildWindow()
{
	//m_wndOutputBuild.AddString(_T("여기에 빌드 출력이 표시됩니다."));
	//m_wndOutputBuild.AddString(_T("출력이 목록 뷰 행에 표시되지만"));
	//m_wndOutputBuild.AddString(_T("표시 방법을 원하는 대로 변경할 수 있습니다."));
}

void COutputWnd::FillDebugWindow()
{
	//m_wndOutputDebug.AddString(_T("여기에 디버그 출력이 표시됩니다."));
	//m_wndOutputDebug.AddString(_T("출력이 목록 뷰 행에 표시되지만"));
	//m_wndOutputDebug.AddString(_T("표시 방법을 원하는 대로 변경할 수 있습니다."));
}

void COutputWnd::FillFindWindow()
{
	m_wndOutputFind.AddString(_T("여기에 찾기 출력이 표시됩니다."));
	m_wndOutputFind.AddString(_T("출력이 목록 뷰 행에 표시되지만"));
	m_wndOutputFind.AddString(_T("표시 방법을 원하는 대로 변경할 수 있습니다."));
}

void COutputWnd::UpdateFonts()
{
	static CFont Font;
	Font.CreateFont(
		19,                        // nHeight 
		0,                         // nWidth 
		0,                         // nEscapement 
		0,                         // nOrientation 
		FW_NORMAL,                 // nWeight 
		FALSE,                     // bItalic 
		FALSE,                     // bUnderline 
		0,                         // cStrikeOut 
		ANSI_CHARSET,              // nCharSet 
		OUT_DEFAULT_PRECIS,        // nOutPrecision 
		CLIP_DEFAULT_PRECIS,       // nClipPrecision 
		DEFAULT_QUALITY,           // nQuality 
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily 
		_T("Arial"));             // lpszFacename
	//m_wndOutputBuild.SetFont(&Font);
	m_wndOutputDebug.SetFont(&Font);
	//m_wndOutputFind.SetFont(&Font);
}

/////////////////////////////////////////////////////////////////////////////
// COutputList1

COutputList::COutputList() noexcept
{
}

COutputList::~COutputList()
{
}

BEGIN_MESSAGE_MAP(COutputList, CListBox)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_COMMAND(ID_VIEW_OUTPUTWND, OnViewOutput)
	ON_WM_DRAWITEM_REFLECT()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEWHEEL()
	ON_WM_VSCROLL()
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// COutputList 메시지 처리기

void COutputList::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	CMenu menu;
	menu.LoadMenu(IDR_OUTPUT_POPUP);

	CMenu* pSumMenu = menu.GetSubMenu(0);

	if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CFrameWndEx)))
	{
		CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

		if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
			return;

		((CFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
		UpdateDialogControls(this, FALSE);
	}

	SetFocus();
}

void COutputList::CopyStringToClipboard(const CString& InString)
{
	if (OpenClipboard()) 
	{
		EmptyClipboard();

		// 문자열 크기와 널 종단 문자를 포함한 버퍼 크기 계산
		HGLOBAL hClipboardData = GlobalAlloc(GMEM_DDESHARE, (InString.GetLength() + 1) * sizeof(TCHAR));

		if (hClipboardData) 
		{
			// 전역 메모리 잠금
			TCHAR* pchData = (TCHAR*)GlobalLock(hClipboardData);

			if (pchData) 
			{
				_tcscpy_s(pchData, InString.GetLength() + 1, (LPCTSTR)InString);
				GlobalUnlock(hClipboardData);

				// 클립보드에 유니코드 텍스트 설정
				SetClipboardData(CF_UNICODETEXT, hClipboardData);
			}
		}
		CloseClipboard();
	}
}
void COutputList::OnEditCopy()
{
	//MessageBox(_T("출력 복사"));
	const int CurSel = GetCurSel();
	if (CurSel < 0) { return; }
	CString CopyString;
	GetText(CurSel, CopyString);
	if (CopyString.IsEmpty()) { return; }
	CopyStringToClipboard(CopyString);
}

void COutputList::OnEditClear()
{
	//MessageBox(_T("출력 지우기"));
	ResetContent();
}

void COutputList::OnViewOutput()
{
	CDockablePane* pParentBar = DYNAMIC_DOWNCAST(CDockablePane, GetOwner());
	CMDIFrameWndEx* pMainFrame = DYNAMIC_DOWNCAST(CMDIFrameWndEx, GetTopLevelFrame());

	if (pMainFrame != nullptr && pParentBar != nullptr)
	{
		pMainFrame->SetFocus();
		pMainFrame->ShowPane(pParentBar, FALSE, FALSE, FALSE);
		pMainFrame->RecalcLayout();
	}
}

//void COutputList::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
//{
//	CDC dc;
//	dc.Attach(lpDrawItemStruct->hDC);
//

//	dc.TextOut(lpDrawItemStruct->rcItem.left, lpDrawItemStruct->rcItem.top,
//		(LPCTSTR)lpDrawItemStruct->itemData);
//
//	dc.Detach();
//}
HBRUSH COutputList::CtlColor(CDC* pDC, UINT nCtlColor)
{
	// 특정 컨트롤에 대한 색상 설정
	if (nCtlColor == CTLCOLOR_LISTBOX)
	{
		pDC->SetTextColor(RGB(255, 255, 255));
		pDC->SetBkColor(RGB(32, 32, 32));
	}

	static CBrush BackgroundBrush(RGB(32, 32, 32));
	return BackgroundBrush;
}

void COutputList::OnPaint()
{
	//CListBox::OnPaint();
	//return;
	{
		CPaintDC dc(this);

		// 부모 창의 폰트 가져오기
		static CFont Font;
		CFont* pFont = GetFont();
		CFont* pOldFont = dc.SelectObject(pFont);

		CRect ClipRect;
		dc.GetClipBox(ClipRect);
		ClipRect.top = 0;
		ClipRect.bottom -= 20;
		CRgn rgn;
		rgn.CreateRectRgnIndirect(&ClipRect);
		dc.SelectClipRgn(&rgn);
		//dc.Rectangle(ClipRect);

		// 텍스트 출력
		for (int i = 0; i < GetCount(); ++i)
		{
			CString LogString;
			GetText(i, LogString);

			CRect Rect;
			GetItemRect(i, &Rect);
			/*if (BoundRect.bottom < Rect.bottom)
			{
				break;
			}*/

			dc.FillSolidRect(&Rect, RGB(32, 32, 32));
			dc.SetBkColor(RGB(32, 32, 32));

			if (i == GetCurSel())
			{
				dc.FillSolidRect(&Rect, RGB(0, 120, 215));
				dc.SetBkColor(RGB(0, 120, 215));
			}
			
			if (LogString.Find(TEXT("Log")) != -1)
			{
				//dc.FillSolidRect(&Rect, RGB(32, 32, 32));
				//dc.SetBkColor(RGB(32, 32, 32)); // 32
				dc.SetTextColor(RGB(255, 255, 255));
			}
			else if (LogString.Find(TEXT("Warning")) != -1)
			{
				//dc.FillSolidRect(&Rect, RGB(255, 255, 0));
				//dc.SetBkColor(RGB(255, 255, 0)); // Yellow
				dc.SetTextColor(RGB(255, 255, 0));
			}
			else if (LogString.Find(TEXT("Error")) != -1)
			{
				//dc.FillSolidRect(&Rect, RGB(255, 0, 0));
				//dc.SetBkColor(RGB(255, 0, 0)); // Red
				dc.SetTextColor(RGB(255, 0, 0));
			}
			else if (LogString.Find(TEXT("Fatal")) != -1)
			{
				//dc.FillSolidRect(&Rect, RGB(255, 0, 255));
				//dc.SetBkColor(RGB(255, 0, 255)); // Magenta
				dc.SetTextColor(RGB(255, 0, 255));
			}
			else
			{
				//dc.FillSolidRect(&Rect, RGB(32, 32, 32));
				//dc.SetBkColor(RGB(32, 32, 32)); // 32
				dc.SetTextColor(RGB(255, 255, 255));
			}

			dc.TextOut(Rect.left, Rect.top, LogString);
		}

		// 이전 폰트 복원
		dc.SelectObject(pOldFont);
	}
}


void COutputList::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	const int LastSel = GetCurSel();
	CRect Rect;
	if (LastSel != LB_ERR)
	{
		GetItemRect(LastSel, &Rect);
	}
	BOOL bResult;
	int NewSelIndex = ItemFromPoint(point, bResult);
	if (NewSelIndex != LB_ERR)
	{
		SetCurSel(NewSelIndex);
	}
	InvalidateRect(NULL, FALSE);
	UpdateWindow();
	//CListBox::OnLButtonDown(nFlags, point);
}

BOOL COutputList::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// zDelta 값에 따라 스크롤 위치를 조정합니다.
	int nScrollLines = zDelta / WHEEL_DELTA; // WHEEL_DELTA는 120으로 정의됨
	int nTopIndex = GetTopIndex();
	int nNewTopIndex = nTopIndex - nScrollLines;

	// 유효한 인덱스 범위 내에서 스크롤 위치를 설정합니다.
	if (nNewTopIndex < 0)
		nNewTopIndex = 0;
	else if (nNewTopIndex >= GetCount())
		nNewTopIndex = GetCount() - 1;

	SetTopIndex(nNewTopIndex);
	InvalidateRect(NULL, FALSE);
	UpdateWindow();

	return TRUE;
}

void COutputList::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	int nTopIndex = GetTopIndex();
	int nNewTopIndex = nTopIndex;

	switch (nSBCode)
	{
	case SB_LINEUP:
		nNewTopIndex = max(nTopIndex - 1, 0);
		break;
	case SB_LINEDOWN:
		nNewTopIndex = min(nTopIndex + 1, GetCount() - 1);
		break;
	case SB_PAGEUP:
		nNewTopIndex = max(nTopIndex - GetCount() / 10, 0); // 한 페이지 위로 스크롤
		break;
	case SB_PAGEDOWN:
		nNewTopIndex = min(nTopIndex + GetCount() / 10, GetCount() - 1); // 한 페이지 아래로 스크롤
		break;
	case SB_THUMBTRACK:
		nNewTopIndex = nPos;
		break;
	}

	SetTopIndex(nNewTopIndex);
	InvalidateRect(NULL, FALSE);
	UpdateWindow();
	//CListBox::OnVScroll(nSBCode, nPos, pScrollBar);
}


#endif // #if WITH_EDITOR

