
// MFCApplication1View.h: CMFCApplicationView 클래스의 인터페이스
//

#pragma once
#if WITH_EDITOR
#include "Misc/MFCHeaders.h"

class CMFCApplicationDoc;
class CMFCApplicationView : public CView
{
protected: // serialization에서만 만들어집니다.
	CMFCApplicationView() noexcept;
	DECLARE_DYNCREATE(CMFCApplicationView)

// 특성입니다.
public:
	//CMFCApplicationDoc* GetDocument() const;

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// 구현입니다.
public:
	virtual ~CMFCApplicationView();
//#ifdef _DEBUG
//	virtual void AssertValid() const;
//	virtual void Dump(CDumpContext& dc) const;
//#endif

protected:
	virtual void OnInitialUpdate() override;

// 생성된 메시지 맵 함수
protected:
	//afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
};

//#ifndef _DEBUG  // MFCApplicationView.cpp의 디버그 버전
//inline CMFCApplicationDoc* CMFCApplicationView::GetDocument() const
//   { return reinterpret_cast<CMFCApplicationDoc*>(m_pDocument); }
//#endif

#endif // #if WITH_EDITOR