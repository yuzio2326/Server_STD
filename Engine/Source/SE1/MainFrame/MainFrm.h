
// MainFrm.h: CMainFrame 클래스의 인터페이스
//

#pragma once
#if WITH_EDITOR
#include "Misc/MFCHeaders.h"
#include "View/FileView.h"
#include "View/WorldOutliner.h"
#include "View/OutputWnd.h"
#include "View/DetailsPanel.h"
#include "UI/afxmenubar_custom.h"

class CMainFrame : public CFrameWndEx
{
	friend class CMFCApplication;
protected: // serialization에서만 만들어집니다.
	CMainFrame() noexcept;
	DECLARE_DYNCREATE(CMainFrame)

// 특성입니다.
public:

// 작업입니다.
public:
	COutputWnd& GetOutputWnd() { return m_wndOutput; }

// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = nullptr, CCreateContext* pContext = nullptr);

// 구현입니다.
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	CDetailsPanel& GetDetailPanel() { return m_wndDetailsPanel; }

public:  // 컨트롤 모음이 포함된 멤버입니다.
	CMFCMenuBar_Custom       m_wndMenuBar;
	CMFCToolBar       m_wndToolBar;
	CMFCStatusBar     m_wndStatusBar;
	CFileView         m_wndFileView;
	CWorldOutliner        m_wndWorldOutliner;
	COutputWnd        m_wndOutput;
	CDetailsPanel    m_wndDetailsPanel;

// 생성된 메시지 맵 함수
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
public:
	afx_msg void OnClose();
};


#endif // #if WITH_EDITOR