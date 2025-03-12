// This MFC Library source code supports the Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

#pragma once
#if WITH_EDITOR
#include "Misc/MFCHeaders.h"
#include "afxvisualmanagerwindows7.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCVisualManagerWindows7_Custom : public CMFCVisualManagerWindows7
{
	DECLARE_DYNCREATE(CMFCVisualManagerWindows7_Custom)

//public:
//	static BOOL __stdcall SetStyle(LPCTSTR lpszPath = NULL);
//	static void __stdcall SetResourceHandle(HINSTANCE hinstRes);
//	static void __stdcall CleanStyle();
//
//protected:
//	static CString __stdcall MakeResourceID(LPCTSTR lpszID);
//	static CString __stdcall GetStyleResourceID();
//
//	BOOL CanDrawImage() const
//	{
//		return GetGlobalData()->m_nBitsPerPixel > 8 && !GetGlobalData()->IsHighContrastMode() && m_bLoaded;
//	}

public:
	CMFCVisualManagerWindows7_Custom();
	virtual ~CMFCVisualManagerWindows7_Custom();

	virtual void OnDrawTab(CDC* pDC, CRect rectTab, int iTab, BOOL bIsActive, const CMFCBaseTabCtrl* pTabWnd);
	virtual void OnDrawTabContent(CDC* pDC, CRect rectTab, int iTab, BOOL bIsActive, const CMFCBaseTabCtrl* pTabWnd, COLORREF clrText);
	virtual void GetTabFrameColors(const CMFCBaseTabCtrl* pTabWnd, COLORREF& clrDark, COLORREF& clrBlack, COLORREF& clrHighlight,
		COLORREF& clrFace, COLORREF& clrDarkShadow, COLORREF& clrLight, CBrush*& pbrFace, CBrush*& pbrBlack);
	//virtual BOOL IsOwnerDrawMenuCheck();
	//virtual BOOL IsHighlightWholeMenuItem();
	//
	//virtual BOOL OnNcActivate(CWnd* pWnd, BOOL bActive);
	//virtual BOOL OnNcPaint(CWnd* pWnd, const CObList& lstSysButtons, CRect rectRedraw);
	
	//virtual void DrawNcBtn(CDC* pDC, const CRect& rect, UINT nButton, AFX_BUTTON_STATE state, BOOL bSmall, BOOL bActive, BOOL bMDI = FALSE);
	//virtual void DrawNcText(CDC* pDC, CRect& rect, const CString& strTitle, BOOL bActive, BOOL bIsRTL, BOOL bTextCenter, BOOL bGlass = FALSE, int nGlassGlowSize = 0, COLORREF clrGlassText = (COLORREF)-1);
	//
	//virtual void OnUpdateSystemColors();
	//virtual void CleanUp();
	//
	//virtual void OnHighlightMenuItem(CDC *pDC, CMFCToolBarMenuButton* pButton, CRect rect, COLORREF& clrText);
	//virtual void OnDrawMenuBorder(CDC* pDC, CMFCPopupMenu* pMenu, CRect rect);
	//virtual void OnDrawMenuCheck(CDC* pDC, CMFCToolBarMenuButton* pButton, CRect rect, BOOL bHighlight, BOOL bIsRadio);

	virtual COLORREF OnDrawMenuLabel(CDC* pDC, CRect rect); 
	virtual COLORREF OnDrawPaneCaption(CDC* pDC, CDockablePane* pBar, BOOL bActive, CRect rectCaption, CRect rectButtons);
	virtual COLORREF GetStatusBarPaneTextColor(CMFCStatusBar* pStatusBar, CMFCStatusBarPaneInfo* pPane);
	virtual COLORREF GetHighlightedMenuItemTextColor(CMFCToolBarMenuButton* pButton);

	virtual COLORREF GetMenuItemTextColor(CMFCToolBarMenuButton* pButton, BOOL bHighlighted, BOOL bDisabled);

	virtual void OnFillAutoHideButtonBackground(CDC* pDC, CRect rect, CMFCAutoHideButton* pButton);
	virtual void OnDrawAutoHideButtonBorder(CDC* pDC, CRect rectBounds, CRect rectBorderSize, CMFCAutoHideButton* pButton);
	virtual COLORREF GetAutoHideButtonTextColor(CMFCAutoHideButton* pButton);
	virtual COLORREF OnDrawPropertySheetListItem(CDC* pDC, CMFCPropertySheet* pParent, CRect rect, BOOL bIsHighlihted, BOOL bIsSelected);

	// Popup window:
	virtual COLORREF OnDrawPopupWindowCaption(CDC* pDC, CRect rectCaption, CMFCDesktopAlertWnd* pPopupWnd);

	
	virtual COLORREF GetToolbarButtonTextColor(CMFCToolBarButton* pButton, CMFCVisualManager::AFX_BUTTON_STATE state) override;
	virtual COLORREF GetToolbarDisabledColor() const { return (COLORREF)-1; }
	virtual COLORREF GetToolbarHighlightColor() { return m_clrHighlight; }
	virtual COLORREF GetToolbarDisabledTextColor();

	virtual void OnFillButtonInterior(CDC* pDC, CMFCToolBarButton* pButton, CRect rect, CMFCVisualManager::AFX_BUTTON_STATE state);

	virtual COLORREF GetPropertyGridGroupColor(CMFCPropertyGridCtrl* pPropList);
	virtual COLORREF GetPropertyGridGroupTextColor(CMFCPropertyGridCtrl* pPropList);
	
	virtual void OnFillBarBackground(CDC* pDC, CBasePane* pBar, CRect rectClient, CRect rectClip, BOOL bNCArea = FALSE);
	
	// Caption bar:
	virtual COLORREF GetCaptionBarTextColor(CMFCCaptionBar* pBar);
	virtual COLORREF OnFillCaptionBarButton(CDC* pDC, CMFCCaptionBar* pBar, CRect rect, BOOL bIsPressed,
		BOOL bIsHighlighted, BOOL bIsDisabled, BOOL bHasDropDownArrow, BOOL bIsSysButton);

	virtual COLORREF GetTabTextColor(const CMFCBaseTabCtrl* pTabWnd, int iTab, BOOL bIsActive);

	// Customization dialog:
	virtual COLORREF OnFillCommandsListBackground(CDC* pDC, CRect rect, BOOL bIsSelected = FALSE);
	virtual COLORREF OnFillMiniFrameCaption(CDC* pDC, CRect rectCaption, CPaneFrameWnd* pFrameWnd, BOOL bActive);

protected:
	CBrush brMyBlack;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
#endif // #if WITH_EDITOR