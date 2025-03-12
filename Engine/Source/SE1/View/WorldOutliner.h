#pragma once

#if WITH_EDITOR
#include "ViewTree.h"
#include <map>
class UObject;
class AActor;
class UWorld;

class CClassToolBar : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CMyViewTree : public CViewTree
{
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnTvnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
};

class CWorldOutliner : public CDockablePane
{
public:
	CWorldOutliner() noexcept;
	virtual ~CWorldOutliner();

	void AdjustLayout();
	void OnChangeVisualStyle();
	void InsertRootItem(const wchar_t* NewName);
	void InsertItem(AActor* ActorPointer, const wchar_t* NewName, int nImage, int nSelectedImage, _In_ HTREEITEM hParent);
	void DeleteAllItems();

	void OnSelectedItemChanged(HTREEITEM NewSelectedItem);
	void OnActorSelectedAndMakeDetails(AActor* SelectedActor);
	void FillDetails(const bool bSameActor, CMFCPropertyGridProperty* ParentUI, UObject* InObject);

protected:
	CClassToolBar m_wndToolBar;
	CMyViewTree m_wndClassView;
	CImageList m_ClassViewImages;
	UINT m_nCurrSort;

	void FillClassView();

private:
	HTREEITEM hClassViewRoot = NULL;
	std::map<HTREEITEM, AActor*> Actors;

public: // 선택된 Actor 관련
	AActor* LastSelectedActor = nullptr;
	std::map<std::wstring, CMFCPropertyGridProperty*> DetailsUI;
	std::map<CMFCPropertyGridProperty*, FPropertyInfo> PropertyInfos;

// 재정의입니다.
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	void OnWorldCreated(UWorld* NewWorld);
	void OnWorldDestroyed(UWorld* NewWorld);
	void OnActorSpawned(AActor* NewActor);
	void OnWorldChanged(UWorld* NewWorld, TArray<TObjectPtr<AActor>>& NewActors);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnClassAddMemberFunction();
	afx_msg void OnClassAddMemberVariable();
	afx_msg void OnClassDefinition();
	afx_msg void OnClassProperties();
	afx_msg void OnNewFolder();
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg LRESULT OnChangeActiveTab(WPARAM, LPARAM);
	afx_msg void OnSort(UINT id);
	afx_msg void OnUpdateSort(CCmdUI* pCmdUI);

	DECLARE_MESSAGE_MAP()
};

#endif // #if WITH_EDITOR