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
#if WITH_EDITOR
#include "afxvisualmanagerwindows7_Custom.h"
#include "afxcontrolbarutil.h"
#include "afxglobalutils.h"
#include "afxvisualmanagerwindows7.h"
#include "afxtoolbar.h"
#include "afxdrawmanager.h"
#include "afxpopupmenubar.h"
#include "afxmenubar.h"
#include "afxglobals.h"
#include "afxtoolbarmenubutton.h"
#include "afxcustomizebutton.h"
#include "afxmenuimages.h"
#include "afxcaptionbar.h"
#include "afxbasetabctrl.h"
#include "afxcolorbar.h"
#include "afxtabctrl.h"
#include "afxtaskspane.h"
#include "afxstatusbar.h"
#include "afxautohidebutton.h"
#include "afxheaderctrl.h"
#include "afxrebar.h"
#include "afxdesktopalertwnd.h"
#include "afxdropdowntoolbar.h"
#include "afxtagmanager.h"
#include "afxframewndex.h"
#include "afxmdiframewndex.h"
#include "afxdockablepane.h"
#include "afxoutlookbartabctrl.h"
#include "afxtoolbarcomboboxbutton.h"

#include "afxribbonbar.h"
#include "afxribbonpanel.h"
#include "afxribboncategory.h"
#include "afxribbonbutton.h"
#include "afxribbonquickaccesstoolbar.h"
#include "afxribboncombobox.h"
#include "afxribbonmainpanel.h"
#include "afxribbonpanelmenu.h"
#include "afxribbonlabel.h"
#include "afxribbonpalettegallery.h"
#include "afxribbonstatusbar.h"
#include "afxribbonstatusbarpane.h"
#include "afxribbonprogressbar.h"
#include "afxribbonlinkctrl.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif

#define AFX_RT_STYLE_XML _T("STYLE_XML")

IMPLEMENT_DYNCREATE(CMFCVisualManagerWindows7_Custom, CMFCVisualManagerWindows7)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMFCVisualManagerWindows7_Custom::CMFCVisualManagerWindows7_Custom()
{
	brMyBlack.CreateSolidBrush(RGB(32, 32, 32));
}

CMFCVisualManagerWindows7_Custom::~CMFCVisualManagerWindows7_Custom()
{
}

void CMFCVisualManagerWindows7_Custom::OnDrawTab(CDC* pDC, CRect rectTab, int iTab, BOOL bIsActive, const CMFCBaseTabCtrl* pTabWnd)
{
	ASSERT_VALID(pTabWnd);
	ASSERT_VALID(pDC);

	//COLORREF clrTab = pTabWnd->GetTabBkColor(iTab);
	COLORREF clrTab = RGB(32, 32, 32);

	CRect rectClip;
	pDC->GetClipBox(rectClip);

	if (pTabWnd->IsFlatTab())
	{
		//----------------
		// Draw tab edges:
		//----------------
#define AFX_FLAT_POINTS_NUM 4
		POINT pts[AFX_FLAT_POINTS_NUM];

		const int nHalfHeight = pTabWnd->GetTabsHeight() / 2;

		if (pTabWnd->GetLocation() == CMFCBaseTabCtrl::LOCATION_BOTTOM)
		{
			rectTab.bottom--;

			pts[0].x = rectTab.left;
			pts[0].y = rectTab.top;

			pts[1].x = rectTab.left + nHalfHeight;
			pts[1].y = rectTab.bottom;

			pts[2].x = rectTab.right - nHalfHeight;
			pts[2].y = rectTab.bottom;

			pts[3].x = rectTab.right;
			pts[3].y = rectTab.top;
		}
		else
		{
			rectTab.top++;

			pts[0].x = rectTab.left + nHalfHeight;
			pts[0].y = rectTab.top;

			pts[1].x = rectTab.left;
			pts[1].y = rectTab.bottom;

			pts[2].x = rectTab.right;
			pts[2].y = rectTab.bottom;

			pts[3].x = rectTab.right - nHalfHeight;
			pts[3].y = rectTab.top;

			rectTab.left += 2;
		}

		CBrush* pOldBrush = NULL;
		CBrush br(clrTab);

		if (!bIsActive && clrTab != (COLORREF)-1)
		{
			pOldBrush = pDC->SelectObject(&br);
		}

		pDC->Polygon(pts, AFX_FLAT_POINTS_NUM);

		if (pOldBrush != NULL)
		{
			pDC->SelectObject(pOldBrush);
		}
	}
	else if (pTabWnd->IsLeftRightRounded())
	{
		CList<POINT, POINT> pts;

		POSITION posLeft = pts.AddHead(CPoint(rectTab.left, rectTab.top));
		posLeft = pts.InsertAfter(posLeft, CPoint(rectTab.left, rectTab.top + 2));

		POSITION posRight = pts.AddTail(CPoint(rectTab.right, rectTab.top));
		posRight = pts.InsertBefore(posRight, CPoint(rectTab.right, rectTab.top + 2));

		int xLeft = rectTab.left + 1;
		int xRight = rectTab.right - 1;

		int y = 0;

		for (y = rectTab.top + 2; y < rectTab.bottom - 4; y += 2)
		{
			posLeft = pts.InsertAfter(posLeft, CPoint(xLeft, y));
			posLeft = pts.InsertAfter(posLeft, CPoint(xLeft, y + 2));

			posRight = pts.InsertBefore(posRight, CPoint(xRight, y));
			posRight = pts.InsertBefore(posRight, CPoint(xRight, y + 2));

			xLeft++;
			xRight--;
		}

		if (pTabWnd->GetLocation() == CMFCBaseTabCtrl::LOCATION_TOP)
		{
			xLeft--;
			xRight++;
		}

		const int nTabLeft = xLeft - 1;
		const int nTabRight = xRight + 1;

		for (; y < rectTab.bottom - 1; y++)
		{
			posLeft = pts.InsertAfter(posLeft, CPoint(xLeft, y));
			posLeft = pts.InsertAfter(posLeft, CPoint(xLeft + 1, y + 1));

			posRight = pts.InsertBefore(posRight, CPoint(xRight, y));
			posRight = pts.InsertBefore(posRight, CPoint(xRight - 1, y + 1));

			if (y == rectTab.bottom - 2)
			{
				posLeft = pts.InsertAfter(posLeft, CPoint(xLeft + 1, y + 1));
				posLeft = pts.InsertAfter(posLeft, CPoint(xLeft + 3, y + 1));

				posRight = pts.InsertBefore(posRight, CPoint(xRight, y + 1));
				posRight = pts.InsertBefore(posRight, CPoint(xRight - 2, y + 1));
			}

			xLeft++;
			xRight--;
		}

		posLeft = pts.InsertAfter(posLeft, CPoint(xLeft + 2, rectTab.bottom));
		posRight = pts.InsertBefore(posRight, CPoint(xRight - 2, rectTab.bottom));

		LPPOINT points = new POINT[pts.GetCount()];

		int i = 0;

		for (POSITION pos = pts.GetHeadPosition(); pos != NULL; i++)
		{
			points[i] = pts.GetNext(pos);

			if (pTabWnd->GetLocation() == CMFCBaseTabCtrl::LOCATION_TOP)
			{
				points[i].y = rectTab.bottom - (points[i].y - rectTab.top);
			}
		}

		CRgn rgnClip;
		rgnClip.CreatePolygonRgn(points, (int)pts.GetCount(), WINDING);

		pDC->SelectClipRgn(&rgnClip);

		CBrush br(clrTab == (COLORREF)-1 ? GetGlobalData()->clrBtnFace : clrTab);
		OnFillTab(pDC, rectTab, &br, iTab, bIsActive, pTabWnd);

		pDC->SelectClipRgn(NULL);

		CPen pen(PS_SOLID, 1, GetGlobalData()->clrBarShadow);
		CPen* pOLdPen = pDC->SelectObject(&pen);

		for (i = 0; i < pts.GetCount(); i++)
		{
			if ((i % 2) != 0)
			{
				int x1 = points[i - 1].x;
				int y1 = points[i - 1].y;

				int x2 = points[i].x;
				int y2 = points[i].y;

				if (x1 > rectTab.CenterPoint().x && x2 > rectTab.CenterPoint().x)
				{
					x1--;
					x2--;
				}

				if (y2 >= y1)
				{
					pDC->MoveTo(x1, y1);
					pDC->LineTo(x2, y2);
				}
				else
				{
					pDC->MoveTo(x2, y2);
					pDC->LineTo(x1, y1);
				}
			}
		}

		delete[] points;
		pDC->SelectObject(pOLdPen);

		rectTab.left = nTabLeft;
		rectTab.right = nTabRight;
	}
	else // 3D Tab
	{
		CRgn rgnClip;

		CRect rectClipTab;
		pTabWnd->GetTabsRect(rectClipTab);

		BOOL bIsCutted = FALSE;

		const BOOL bIsOneNote = pTabWnd->IsOneNoteStyle() || pTabWnd->IsVS2005Style();
		const int nExtra = bIsOneNote ? ((pTabWnd->IsFirstTab(iTab) || bIsActive || pTabWnd->IsVS2005Style()) ? 0 : rectTab.Height()) : 0;

		if (rectTab.left + nExtra + 10 > rectClipTab.right || rectTab.right - 10 <= rectClipTab.left)
		{
			return;
		}

		const int iVertOffset = 2;
		const int iHorzOffset = 2;
		const BOOL bIs2005 = pTabWnd->IsVS2005Style();

#define AFX_POINTS_NUM 8
		POINT pts[AFX_POINTS_NUM];

		if (!bIsActive || bIsOneNote || clrTab != (COLORREF)-1 || m_bAlwaysFillTab)
		{
			if (clrTab != (COLORREF)-1 || bIsOneNote || m_bAlwaysFillTab)
			{
				CRgn rgn;
				CBrush br(clrTab == (COLORREF)-1 ? GetGlobalData()->clrBtnFace : clrTab);

				CRect rectFill = rectTab;

				if (bIsOneNote)
				{
					CRect rectFillTab = rectTab;

					const int nHeight = rectFillTab.Height();

					pts[0].x = rectFillTab.left;
					pts[0].y = rectFillTab.bottom;

					pts[1].x = rectFillTab.left;
					pts[1].y = rectFillTab.bottom;

					pts[2].x = rectFillTab.left + 2;
					pts[2].y = rectFillTab.bottom;

					pts[3].x = rectFillTab.left + nHeight;
					pts[3].y = rectFillTab.top + 2;

					pts[4].x = rectFillTab.left + nHeight + 4;
					pts[4].y = rectFillTab.top;

					pts[5].x = rectFillTab.right - 2;
					pts[5].y = rectFillTab.top;

					pts[6].x = rectFillTab.right;
					pts[6].y = rectFillTab.top + 2;

					pts[7].x = rectFillTab.right;
					pts[7].y = rectFillTab.bottom;

					for (int i = 0; i < AFX_POINTS_NUM; i++)
					{
						if (pts[i].x > rectClipTab.right)
						{
							pts[i].x = rectClipTab.right;
							bIsCutted = TRUE;
						}

						if (pTabWnd->GetLocation() == CMFCBaseTabCtrl::LOCATION_BOTTOM)
						{
							pts[i].y = rectFillTab.bottom - pts[i].y + rectFillTab.top - 1;
						}
					}

					rgn.CreatePolygonRgn(pts, AFX_POINTS_NUM, WINDING);
					pDC->SelectClipRgn(&rgn);
				}
				else
				{
					rectFill.DeflateRect(1, 0);

					if (pTabWnd->GetLocation() == CMFCBaseTabCtrl::LOCATION_BOTTOM)
					{
						rectFill.bottom--;
					}
					else
					{
						rectFill.top++;
					}

					rectFill.right = min(rectFill.right, rectClipTab.right);
				}
				const_cast<CMFCBaseTabCtrl*>(pTabWnd)->SetTabBkColor(iTab, RGB(32, 32, 32));
				OnFillTab(pDC, rectFill, &br, iTab, bIsActive, pTabWnd);
				pDC->SelectClipRgn(NULL);

				if (bIsOneNote)
				{
					CRect rectLeft;
					pTabWnd->GetClientRect(rectLeft);
					rectLeft.right = rectClipTab.left - 1;

					pDC->ExcludeClipRect(rectLeft);

					if (!pTabWnd->IsFirstTab(iTab) && !bIsActive && iTab != pTabWnd->GetFirstVisibleTabNum())
					{
						CRect rectLeftTab = rectClipTab;
						rectLeftTab.right = rectFill.left + rectFill.Height() - 10;

						const int nVertOffset = bIs2005 ? 2 : 1;

						if (pTabWnd->GetLocation() == CMFCBaseTabCtrl::LOCATION_BOTTOM)
						{
							rectLeftTab.top -= nVertOffset;
						}
						else
						{
							rectLeftTab.bottom += nVertOffset;
						}

						pDC->ExcludeClipRect(rectLeftTab);
					}

					pDC->Polyline(pts, AFX_POINTS_NUM);

					if (bIsCutted)
					{
						pDC->MoveTo(rectClipTab.right, rectTab.top);
						pDC->LineTo(rectClipTab.right, rectTab.bottom);
					}

					CRect rectRight = rectClipTab;
					rectRight.left = rectFill.right;

					pDC->ExcludeClipRect(rectRight);
				}
			}
		}

		//CPen penLight(PS_SOLID, 1, GetGlobalData()->clrBarHilite);
		CPen penLight(PS_SOLID, 1, RGB(32, 32, 32));
		//CPen penShadow(PS_SOLID, 1, GetGlobalData()->clrBarShadow);
		CPen penShadow(PS_SOLID, 1, RGB(16, 16, 16));
		//CPen penDark(PS_SOLID, 1, GetGlobalData()->clrBarDkShadow);
		CPen penDark(PS_SOLID, 1, RGB(0, 0, 0));

		CPen* pOldPen = NULL;

		if (bIsOneNote)
		{
			pOldPen = (CPen*)pDC->SelectObject(&penLight);
			ENSURE(pOldPen != NULL);

			if (pTabWnd->GetLocation() == CMFCBaseTabCtrl::LOCATION_BOTTOM)
			{
				if (!bIsCutted)
				{
					int yTop = bIsActive ? pts[7].y - 1 : pts[7].y;

					pDC->MoveTo(pts[6].x - 1, pts[6].y);
					pDC->LineTo(pts[7].x - 1, yTop);
				}
			}
			else
			{
				pDC->MoveTo(pts[2].x + 1, pts[2].y);
				pDC->LineTo(pts[3].x + 1, pts[3].y);

				pDC->MoveTo(pts[3].x + 1, pts[3].y);
				pDC->LineTo(pts[3].x + 2, pts[3].y);

				pDC->MoveTo(pts[3].x + 2, pts[3].y);
				pDC->LineTo(pts[3].x + 3, pts[3].y);

				pDC->MoveTo(pts[4].x - 1, pts[4].y + 1);
				pDC->LineTo(pts[5].x + 1, pts[5].y + 1);

				if (!bIsActive && !bIsCutted && m_b3DTabWideBorder)
				{
					pDC->SelectObject(&penShadow);

					pDC->MoveTo(pts[6].x - 2, pts[6].y - 1);
					pDC->LineTo(pts[6].x - 1, pts[6].y - 1);
				}

				pDC->MoveTo(pts[6].x - 1, pts[6].y);
				pDC->LineTo(pts[7].x - 1, pts[7].y);
			}
		}
		else
		{
			if (rectTab.right > rectClipTab.right)
			{
				CRect rectTabClip = rectTab;
				rectTabClip.right = rectClipTab.right;

				rgnClip.CreateRectRgnIndirect(&rectTabClip);
				pDC->SelectClipRgn(&rgnClip);
			}

			if (pTabWnd->GetLocation() == CMFCBaseTabCtrl::LOCATION_BOTTOM)
			{
				pOldPen = (CPen*)pDC->SelectObject(&penLight);
				ENSURE(pOldPen != NULL);

				if (!m_b3DTabWideBorder)
				{
					pDC->SelectObject(&penShadow);
				}

				pDC->MoveTo(rectTab.left, rectTab.top);
				pDC->LineTo(rectTab.left, rectTab.bottom - iVertOffset);

				if (m_b3DTabWideBorder)
				{
					pDC->SelectObject(&penDark);
				}

				pDC->LineTo(rectTab.left + iHorzOffset, rectTab.bottom);
				pDC->LineTo(rectTab.right - iHorzOffset, rectTab.bottom);
				pDC->LineTo(rectTab.right, rectTab.bottom - iVertOffset);
				pDC->LineTo(rectTab.right, rectTab.top - 1);

				pDC->SelectObject(&penShadow);

				if (m_b3DTabWideBorder)
				{
					pDC->MoveTo(rectTab.left + iHorzOffset + 1, rectTab.bottom - 1);
					pDC->LineTo(rectTab.right - iHorzOffset, rectTab.bottom - 1);
					pDC->LineTo(rectTab.right - 1, rectTab.bottom - iVertOffset);
					pDC->LineTo(rectTab.right - 1, rectTab.top - 1);
				}
			}
			else
			{
				pOldPen = pDC->SelectObject(m_b3DTabWideBorder ? &penDark : &penShadow);

				ENSURE(pOldPen != NULL);

				pDC->MoveTo(rectTab.right, bIsActive ? rectTab.bottom : rectTab.bottom - 1);
				pDC->LineTo(rectTab.right, rectTab.top + iVertOffset);
				pDC->LineTo(rectTab.right - iHorzOffset, rectTab.top);

				if (m_b3DTabWideBorder)
				{
					pDC->SelectObject(&penLight);
				}

				pDC->LineTo(rectTab.left + iHorzOffset, rectTab.top);
				pDC->LineTo(rectTab.left, rectTab.top + iVertOffset);

				pDC->LineTo(rectTab.left, rectTab.bottom);

				if (m_b3DTabWideBorder)
				{
					pDC->SelectObject(&penShadow);

					pDC->MoveTo(rectTab.right - 1, bIsActive ? rectTab.bottom : rectTab.bottom - 1);
					pDC->LineTo(rectTab.right - 1, rectTab.top + iVertOffset - 1);
				}
			}
		}

		if (bIsActive)
		{
			const int iBarHeight = 1;
			const int y = (pTabWnd->GetLocation() == CMFCBaseTabCtrl::LOCATION_BOTTOM) ? (rectTab.top - iBarHeight - 1) : (rectTab.bottom);

			CRect rectFill(CPoint(rectTab.left, y), CSize(rectTab.Width(), iBarHeight + 1));

			COLORREF clrActiveTab = pTabWnd->GetTabBkColor(iTab);

			if (bIsOneNote)
			{
				if (bIs2005)
				{
					rectFill.left += 3;
				}
				else
				{
					rectFill.OffsetRect(1, 0);
					rectFill.left++;
				}

				if (clrActiveTab == (COLORREF)-1)
				{
					clrActiveTab = GetGlobalData()->clrWindow;
				}
			}

			if (clrActiveTab != (COLORREF)-1)
			{
				CBrush br(clrActiveTab);
				pDC->FillRect(rectFill, &br);
			}
			else
			{
				pDC->FillRect(rectFill, &(GetGlobalData()->brBarFace));
			}
		}

		pDC->SelectObject(pOldPen);

		if (bIsOneNote)
		{
			const int nLeftMargin = pTabWnd->IsVS2005Style() && bIsActive ? rectTab.Height() * 3 / 4 : rectTab.Height();
			const int nRightMargin = pTabWnd->IsVS2005Style() && bIsActive ? CMFCBaseTabCtrl::AFX_TAB_IMAGE_MARGIN * 3 / 4 : CMFCBaseTabCtrl::AFX_TAB_IMAGE_MARGIN;

			rectTab.left += nLeftMargin;
			rectTab.right -= nRightMargin;

			if (pTabWnd->IsVS2005Style() && bIsActive && pTabWnd->HasImage(iTab))
			{
				rectTab.OffsetRect(CMFCBaseTabCtrl::AFX_TAB_IMAGE_MARGIN, 0);
			}
		}

		pDC->SelectClipRgn(NULL);
	}

	COLORREF clrText = pTabWnd->GetTabTextColor(iTab);

	COLORREF clrTextOld = (COLORREF)-1;
	if (!bIsActive /*&& clrText != (COLORREF)-1*/)
	{
		clrTextOld = pDC->SetTextColor(RGB(128, 128, 128));
	}
	else
	{
		clrTextOld = pDC->SetTextColor(RGB(255, 255, 255));
	}

	if (pTabWnd->IsOneNoteStyle() || pTabWnd->IsVS2005Style())
	{
		CRect rectClipTab;
		pTabWnd->GetTabsRect(rectClipTab);

		rectTab.right = min(rectTab.right, rectClipTab.right - 2);
	}

	CRgn rgn;
	rgn.CreateRectRgnIndirect(rectClip);

	pDC->SelectClipRgn(&rgn);

	OnDrawTabContent(pDC, rectTab, iTab, bIsActive, pTabWnd, (COLORREF)-1);

	//if (clrTextOld != (COLORREF)-1)
	{
		pDC->SetTextColor(clrTextOld);
	}

	pDC->SelectClipRgn(NULL);
}

void CMFCVisualManagerWindows7_Custom::OnDrawTabContent(CDC* pDC, CRect rectTab, int iTab, BOOL bIsActive, const CMFCBaseTabCtrl* pTabWnd, COLORREF clrText)
{
	ASSERT_VALID(pTabWnd);
	ASSERT_VALID(pDC);

	if (pTabWnd->IsActiveTabCloseButton() && bIsActive)
	{
		CRect rectClose = pTabWnd->GetTabCloseButton();
		rectTab.right = rectClose.left;

		OnDrawTabCloseButton(pDC, rectClose, pTabWnd, pTabWnd->IsTabCloseButtonHighlighted(), pTabWnd->IsTabCloseButtonPressed(), FALSE /* Disabled */);
	}

	CString strText;
	pTabWnd->GetTabLabel(iTab, strText);

	//if (pTabWnd->IsFlatTab())
	if (true)
	{
		//---------------
		// Draw tab text:
		//---------------
		UINT nFormat = DT_SINGLELINE | DT_CENTER | DT_VCENTER;
		if (pTabWnd->IsDrawNoPrefix())
		{
			nFormat |= DT_NOPREFIX;
		}

		AdjustTabTextRect(rectTab);
		pDC->DrawText(strText, rectTab, nFormat);
	}
	else
	{
		CSize sizeImage = pTabWnd->GetImageSize();
		UINT uiIcon = pTabWnd->GetTabIcon(iTab);
		HICON hIcon = pTabWnd->GetTabHicon(iTab);

		if (uiIcon == (UINT)-1 && hIcon == NULL)
		{
			sizeImage.cx = 0;
		}

		if (sizeImage.cx + 2 * CMFCBaseTabCtrl::AFX_TAB_IMAGE_MARGIN <= rectTab.Width())
		{
			if (hIcon != NULL)
			{
				//---------------------
				// Draw the tab's icon:
				//---------------------
				CRect rectImage = rectTab;

				rectImage.top += (rectTab.Height() - sizeImage.cy) / 2;
				rectImage.bottom = rectImage.top + sizeImage.cy;

				rectImage.left += AFX_IMAGE_MARGIN;
				rectImage.right = rectImage.left + sizeImage.cx;

				pDC->DrawState(rectImage.TopLeft(), rectImage.Size(), hIcon, DSS_NORMAL, (HBRUSH)NULL);
			}
			else
			{
				const CImageList* pImageList = pTabWnd->GetImageList();
				if (pImageList != NULL && uiIcon != (UINT)-1)
				{
					//----------------------
					// Draw the tab's image:
					//----------------------
					CRect rectImage = rectTab;

					rectImage.top += (rectTab.Height() - sizeImage.cy) / 2;
					rectImage.bottom = rectImage.top + sizeImage.cy;

					rectImage.left += AFX_IMAGE_MARGIN;
					rectImage.right = rectImage.left + sizeImage.cx;

					ASSERT_VALID(pImageList);
					((CImageList*)pImageList)->Draw(pDC, uiIcon, rectImage.TopLeft(), ILD_TRANSPARENT);
				}
			}

			//------------------------------
			// Finally, draw the tab's text:
			//------------------------------
			CRect rcText = rectTab;
			rcText.left += sizeImage.cx + 2 * AFX_TEXT_MARGIN;

			if (rcText.Width() < sizeImage.cx * 2 && !pTabWnd->IsLeftRightRounded())
			{
				rcText.right -= AFX_TEXT_MARGIN;
			}

			if (clrText == (COLORREF)-1)
			{
				clrText = GetTabTextColor(pTabWnd, iTab, bIsActive);
			}

			if (clrText != (COLORREF)-1)
			{
				pDC->SetTextColor(clrText);
			}

			UINT nFormat = DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS;
			if (pTabWnd->IsDrawNoPrefix())
			{
				nFormat |= DT_NOPREFIX;
			}

			if (pTabWnd->IsOneNoteStyle() || pTabWnd->IsVS2005Style())
			{
				nFormat |= DT_CENTER;
			}
			else
			{
				nFormat |= DT_LEFT;
			}

			pDC->DrawText(strText, rcText, nFormat);
		}
	}
}

void CMFCVisualManagerWindows7_Custom::GetTabFrameColors(const CMFCBaseTabCtrl* pTabWnd, COLORREF& clrDark, COLORREF& clrBlack, COLORREF& clrHighlight, COLORREF& clrFace, COLORREF& clrDarkShadow, COLORREF& clrLight, CBrush*& pbrFace, CBrush*& pbrBlack)
{
	ASSERT_VALID(pTabWnd);

	/*COLORREF clrActiveTab = pTabWnd->GetTabBkColor(pTabWnd->GetActiveTab());

	if (pTabWnd->IsOneNoteStyle() && clrActiveTab != (COLORREF)-1)
	{
		clrFace = clrActiveTab;
	}
	else if (pTabWnd->IsDialogControl())
	{
		clrFace = GetGlobalData()->clrBtnFace;
	}
	else
	{
		clrFace = GetGlobalData()->clrBarFace;
	}

	if (pTabWnd->IsDialogControl())
	{
		clrDark = GetGlobalData()->clrBtnShadow;
		clrBlack = GetGlobalData()->clrBtnText;
		clrHighlight = pTabWnd->IsVS2005Style() ? GetGlobalData()->clrBtnShadow : GetGlobalData()->clrBtnHilite;
		clrDarkShadow = GetGlobalData()->clrBtnDkShadow;
		clrLight = GetGlobalData()->clrBtnLight;

		pbrFace = &(GetGlobalData()->brBtnFace);
	}
	else
	{
		clrDark = GetGlobalData()->clrBarShadow;
		clrBlack = GetGlobalData()->clrBarText;
		clrHighlight = pTabWnd->IsVS2005Style() ? GetGlobalData()->clrBarShadow : GetGlobalData()->clrBarHilite;
		clrDarkShadow = GetGlobalData()->clrBarDkShadow;
		clrLight = GetGlobalData()->clrBarLight;

		pbrFace = &(GetGlobalData()->brBarFace);
	}*/

	// My custom
	{
		pbrFace = &brMyBlack;

		clrFace = RGB(0, 0, 0);

		clrDark = RGB(16, 16, 16);
		clrBlack = RGB(0, 0, 0);
		clrHighlight = RGB(64, 64, 64);
		clrDarkShadow = RGB(16, 16, 16);
		clrLight = RGB(32, 32, 32);


		//pbrBlack = &(GetGlobalData()->brBlack);
		pbrBlack = &brMyBlack;
	}
}

COLORREF CMFCVisualManagerWindows7_Custom::GetToolbarDisabledTextColor()
{
	return COLORREF(-1);
}

void CMFCVisualManagerWindows7_Custom::OnFillButtonInterior(CDC* pDC, CMFCToolBarButton* pButton, CRect rect, CMFCVisualManager::AFX_BUTTON_STATE state)
{
	pButton->m_strText;
	if (!CanDrawImage())
	{
		CMFCVisualManagerWindows::OnFillButtonInterior(pDC, pButton, rect, state);
		if (pButton)
		{
			CDrawingManager dm(*pDC);
			dm.FillGradient(rect, RGB(32, 32, 32), RGB(32, 32, 32), TRUE);
		}
		return;
	}

	CMFCCustomizeButton* pCustButton = DYNAMIC_DOWNCAST(CMFCCustomizeButton, pButton);

	if (pCustButton == NULL)
	{
		if (CMFCToolBar::IsCustomizeMode() &&
			!CMFCToolBar::IsAltCustomizeMode() && !pButton->IsLocked())
		{
			return;
		}

		CMFCControlRenderer* pRenderer = NULL;
		int index = 0;

		BOOL bDisabled = (pButton->m_nStyle & TBBS_DISABLED) == TBBS_DISABLED;
		//BOOL bPressed  = (pButton->m_nStyle & TBBS_PRESSED ) == TBBS_PRESSED;
		BOOL bChecked = (pButton->m_nStyle & TBBS_CHECKED) == TBBS_CHECKED;

		CBasePane* pBar = DYNAMIC_DOWNCAST(CBasePane, pButton->GetParentWnd());

		CMFCToolBarMenuButton* pMenuButton =
			DYNAMIC_DOWNCAST(CMFCToolBarMenuButton, pButton);
		if (pMenuButton != NULL && pBar != NULL)
		{
			if (pBar->IsKindOf(RUNTIME_CLASS(CMFCMenuBar)) == NULL && pBar->IsKindOf(RUNTIME_CLASS(CMFCPopupMenuBar)))
			{
				if (bChecked)
				{
					pRenderer = &m_ctrlMenuItemBack;

					if (bDisabled)
					{
						index = 1;
					}

					rect.InflateRect(0, 0, 0, 1);
				}
				else if (state == ButtonsIsPressed || state == ButtonsIsHighlighted)
				{
					pRenderer = &m_ctrlMenuHighlighted[bDisabled ? 1 : 0];
				}
				else
				{
					return;
				}
			}
		}

		if (pRenderer != NULL)
		{
			pRenderer->Draw(pDC, rect, index);
			return;
		}
	}

	CMFCVisualManagerWindows::OnFillButtonInterior(pDC, pButton, rect, state);

	if (pButton)
	{
		CDrawingManager dm(*pDC);
		dm.FillGradient(rect, RGB(32, 32, 32), RGB(32, 32, 32), TRUE);
	}
}

COLORREF CMFCVisualManagerWindows7_Custom::GetPropertyGridGroupColor(CMFCPropertyGridCtrl* pPropList)
{
	return COLORREF(RGB(32, 32, 32));
}

COLORREF CMFCVisualManagerWindows7_Custom::GetPropertyGridGroupTextColor(CMFCPropertyGridCtrl* pPropList)
{
	return COLORREF(RGB(255, 255, 255));
}

COLORREF CMFCVisualManagerWindows7_Custom::OnDrawPaneCaption(CDC* pDC, CDockablePane* pBar, BOOL bActive, CRect rectCaption, CRect rectButtons)
{
	return COLORREF(RGB(255, 255, 255));
}

COLORREF CMFCVisualManagerWindows7_Custom::GetHighlightedMenuItemTextColor(CMFCToolBarMenuButton* pButton)
{
	return COLORREF();
}

COLORREF CMFCVisualManagerWindows7_Custom::GetMenuItemTextColor(CMFCToolBarMenuButton* pButton, BOOL bHighlighted, BOOL bDisabled)
{
	if (bHighlighted)
	{
		return bDisabled ? GetGlobalData()->clrBtnFace : GetGlobalData()->clrTextHilite;
	}

	return bDisabled ? GetGlobalData()->clrGrayedText : COLORREF(RGB(255, 255, 255));
}

void CMFCVisualManagerWindows7_Custom::OnFillAutoHideButtonBackground(CDC* pDC, CRect rect, CMFCAutoHideButton* pButton)
{
	ASSERT_VALID(pDC);

	HBRUSH Brush = CreateSolidBrush(RGB(32, 32, 32));
	//::FillRect(pDC->GetSafeHdc(), rectClient, ::GetSysColorBrush(COLOR_MENU));
	::FillRect(pDC->GetSafeHdc(), rect, Brush);
	DeleteObject(Brush);
	//pDC->FillRect(rect, &(GetGlobalData()->brBarFace));
}

void CMFCVisualManagerWindows7_Custom::OnDrawAutoHideButtonBorder(CDC* pDC, CRect rectBounds, CRect rectBorderSize, CMFCAutoHideButton* pButton)
{
	ASSERT_VALID(pDC);

	COLORREF clr = GetGlobalData()->clrBarShadow;
	COLORREF clrText = pDC->GetTextColor();

	if (rectBorderSize.left > 0)
	{
		pDC->FillSolidRect(rectBounds.left, rectBounds.top, rectBounds.left + rectBorderSize.left, rectBounds.bottom, clr);
	}
	if (rectBorderSize.top > 0)
	{
		pDC->FillSolidRect(rectBounds.left, rectBounds.top, rectBounds.right, rectBounds.top + rectBorderSize.top, clr);
	}
	if (rectBorderSize.right > 0)
	{
		pDC->FillSolidRect(rectBounds.right - rectBorderSize.right, rectBounds.top, rectBounds.right, rectBounds.bottom, clr);
	}
	if (rectBorderSize.bottom > 0)
	{
		pDC->FillSolidRect(rectBounds.left, rectBounds.bottom - rectBorderSize.bottom, rectBounds.right, rectBounds.bottom, clr);
	}

	pDC->SetTextColor(clrText);
}

COLORREF CMFCVisualManagerWindows7_Custom::GetStatusBarPaneTextColor(CMFCStatusBar* pStatusBar, CMFCStatusBarPaneInfo* pPane)
{
	return(pPane->nStyle & SBPS_DISABLED) ? GetGlobalData()->clrGrayedText : COLORREF(RGB(255, 255, 255));
}

COLORREF CMFCVisualManagerWindows7_Custom::OnFillMiniFrameCaption(CDC* pDC, CRect rectCaption, CPaneFrameWnd* pFrameWnd, BOOL bActive)
{
	return COLORREF(RGB(32,32,32));
}

COLORREF CMFCVisualManagerWindows7_Custom::OnFillCommandsListBackground(CDC* pDC, CRect rect, BOOL bIsSelected)
{
	return COLORREF();
}

COLORREF CMFCVisualManagerWindows7_Custom::GetCaptionBarTextColor(CMFCCaptionBar* pBar)
{
	return COLORREF();
}

COLORREF CMFCVisualManagerWindows7_Custom::OnFillCaptionBarButton(CDC* pDC, CMFCCaptionBar* pBar, CRect rect, BOOL bIsPressed, BOOL bIsHighlighted, BOOL bIsDisabled, BOOL bHasDropDownArrow, BOOL bIsSysButton)
{
	return COLORREF();
}

COLORREF CMFCVisualManagerWindows7_Custom::GetTabTextColor(const CMFCBaseTabCtrl* pTabWnd, int iTab, BOOL bIsActive)
{
	return COLORREF();
}

COLORREF CMFCVisualManagerWindows7_Custom::GetAutoHideButtonTextColor(CMFCAutoHideButton* pButton)
{
	//return CMFCVisualManagerWindows7::GetAutoHideButtonTextColor(pButton);
	return COLORREF(RGB(255, 255, 255));
}

COLORREF CMFCVisualManagerWindows7_Custom::OnDrawPropertySheetListItem(CDC* pDC, CMFCPropertySheet* pParent, CRect rect, BOOL bIsHighlihted, BOOL bIsSelected)
{
	return COLORREF();
}

COLORREF CMFCVisualManagerWindows7_Custom::OnDrawPopupWindowCaption(CDC* pDC, CRect rectCaption, CMFCDesktopAlertWnd* pPopupWnd)
{
	return COLORREF();
}

COLORREF CMFCVisualManagerWindows7_Custom::OnDrawMenuLabel(CDC* pDC, CRect rect)
{
	return COLORREF();
}

COLORREF CMFCVisualManagerWindows7_Custom::GetToolbarButtonTextColor(CMFCToolBarButton* pButton, CMFCVisualManager::AFX_BUTTON_STATE state)
{
	ASSERT_VALID(pButton);

	BOOL bDisabled = (CMFCToolBar::IsCustomizeMode() && !pButton->IsEditable()) || (!CMFCToolBar::IsCustomizeMode() && (pButton->m_nStyle & TBBS_DISABLED));

	if (pButton->IsKindOf(RUNTIME_CLASS(CMFCOutlookBarPaneButton)))
	{
		if (GetGlobalData()->IsHighContrastMode())
		{
			return bDisabled ? GetGlobalData()->clrGrayedText : GetGlobalData()->clrWindowText;
		}

		return bDisabled ? GetGlobalData()->clrBtnFace : GetGlobalData()->clrWindow;
	}

	//return(bDisabled ? GetGlobalData()->clrGrayedText : (state == ButtonsIsHighlighted) ? CMFCToolBar::GetHotTextColor() : GetGlobalData()->clrBarText);
	return(bDisabled ? GetGlobalData()->clrGrayedText : (state == ButtonsIsHighlighted) ? RGB(255, 255, 100) : RGB(255, 255, 255));
}

void CMFCVisualManagerWindows7_Custom::OnFillBarBackground(CDC* pDC, CBasePane* pBar, CRect rectClient, CRect rectClip, BOOL bNCArea)
{
	ASSERT_VALID(pDC);
	ASSERT_VALID(pBar);

	if (!CanDrawImage())
	{
		CMFCVisualManagerWindows::OnFillBarBackground(pDC, pBar, rectClient, rectClip, bNCArea);
		//return;
	}

	if (pBar->IsKindOf(RUNTIME_CLASS(CMFCRibbonStatusBar)))
	{
		if (m_hThemeWindow != NULL)
		{
			DrawThemeBackground(m_hThemeStatusBar, pDC->GetSafeHdc(), 0, 0, &rectClient, 0);
			//return;
		}
	}
	else if (pBar->IsKindOf(RUNTIME_CLASS(CMFCRibbonBar)))
	{
		CDrawingManager dm(*pDC);
		dm.FillGradient(rectClient, m_clrRibbonBarGradientDark, m_clrRibbonBarGradientLight, TRUE);

		//return;
	}

	ASSERT_VALID(pBar);
	ASSERT_VALID(pDC);

	if (pBar->IsKindOf(RUNTIME_CLASS(CMFCStatusBar)) && m_hThemeStatusBar != NULL)
	{
		DrawThemeBackground(m_hThemeStatusBar, pDC->GetSafeHdc(), 0, 0, &rectClient, 0);
		//return;
	}

	if (pBar->IsKindOf(RUNTIME_CLASS(CMFCMenuBar)))
	{
		CMFCMenuBar* pMenuBar = DYNAMIC_DOWNCAST(CMFCMenuBar, pBar);
		if (pMenuBar)
		{
			//pMenuBar->Getmenus();
			HBRUSH Brush = CreateSolidBrush(RGB(32, 32, 32));
			//::FillRect(pDC->GetSafeHdc(), rectClient, ::GetSysColorBrush(COLOR_MENU));
			::FillRect(pDC->GetSafeHdc(), rectClient, Brush);
			DeleteObject(Brush);
		}
		return;
	}
	if (pBar->IsKindOf(RUNTIME_CLASS(CMFCRibbonStatusBar)))
	{
		if (m_hThemeStatusBar != NULL)
		{
			DrawThemeBackground(m_hThemeStatusBar, pDC->GetSafeHdc(), 0, 0, &rectClient, 0);
			//return;
		}
	}

	if (m_hThemeRebar == NULL || pBar->IsDialogControl() || pBar->IsKindOf(RUNTIME_CLASS(CMFCCaptionBar)) || pBar->IsKindOf(RUNTIME_CLASS(CMFCColorBar)))
	{
		CMFCVisualManagerOfficeXP::OnFillBarBackground(pDC, pBar, rectClient, rectClip, bNCArea);
		//return;
	}

	// Menu 클릭 후 나오는 바
	if (pBar->IsKindOf(RUNTIME_CLASS(CMFCPopupMenuBar)))
	{
		if (m_bOfficeStyleMenus)
		{
			CMFCVisualManagerOfficeXP::OnFillBarBackground(pDC, pBar, rectClient, rectClip, bNCArea);
		}
		else
		{
			{
				HBRUSH Brush = CreateSolidBrush(RGB(32, 32, 32));
				//::FillRect(pDC->GetSafeHdc(), rectClient, ::GetSysColorBrush(COLOR_MENU));
				::FillRect(pDC->GetSafeHdc(), rectClient, Brush);
				DeleteObject(Brush);
			}
			
			CMFCPopupMenuBar* pMenuBar = DYNAMIC_DOWNCAST(CMFCPopupMenuBar, pBar);
			//pMenuBar->SetHotTextColor(RGB(255, 255, 255));
			if (m_hThemeMenu != NULL && !pMenuBar->m_bDisableSideBarInXPMode)
			{
				CRect rectGutter = rectClient;
				rectGutter.right = rectGutter.left + pMenuBar->GetGutterWidth() + 2;
				rectGutter.DeflateRect(0, 1);
				// Menu 클릭 후 나오는 왼쪽 이미지 바
				HBRUSH Brush = CreateSolidBrush(RGB(50, 50, 50));
				::FillRect(pDC->GetSafeHdc(), rectGutter, Brush);
				DeleteObject(Brush);
				//DrawThemeBackground(m_hThemeMenu, pDC->GetSafeHdc(), MENU_POPUPGUTTER, 0, &rectGutter, 0);

				return;
			}
		}

		return;
	}

	FillReBarPane(pDC, pBar, rectClient);

	CDrawingManager dm(*pDC);
	dm.FillGradient(rectClient, RGB(32, 32, 32), RGB(32, 32, 32), TRUE);
}
#endif // #if WITH_EDITOR