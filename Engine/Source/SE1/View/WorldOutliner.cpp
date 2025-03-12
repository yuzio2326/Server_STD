#if WITH_EDITOR
#include "Misc/MFCHeaders.h"
#include "MainFrame/MainFrm.h"
#include "WorldOutliner.h"
#include "Resource/Resource.h"
#include "Application.h"
#include "EngineMinimal.h"
#include "Engine/World.h"

class CClassViewMenuButton : public CMFCToolBarMenuButton
{
	friend class CWorldOutliner;

	DECLARE_SERIAL(CClassViewMenuButton)

public:
	CClassViewMenuButton(HMENU hMenu = nullptr) noexcept : CMFCToolBarMenuButton((UINT)-1, hMenu, -1)
	{
	}

	virtual void OnDraw(CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages, BOOL bHorz = TRUE,
		BOOL bCustomizeMode = FALSE, BOOL bHighlight = FALSE, BOOL bDrawBorder = TRUE, BOOL bGrayDisabledButtons = TRUE)
	{
		pImages = CMFCToolBar::GetImages();

		CAfxDrawState ds;
		pImages->PrepareDrawImage(ds);

		CMFCToolBarMenuButton::OnDraw(pDC, rect, pImages, bHorz, bCustomizeMode, bHighlight, bDrawBorder, bGrayDisabledButtons);

		pImages->EndDrawImage(ds);
	}
};

IMPLEMENT_SERIAL(CClassViewMenuButton, CMFCToolBarMenuButton, 1)

//////////////////////////////////////////////////////////////////////
// 생성/소멸
//////////////////////////////////////////////////////////////////////

CWorldOutliner::CWorldOutliner() noexcept
{
	m_nCurrSort = ID_SORTING_GROUPBYTYPE;
}

CWorldOutliner::~CWorldOutliner()
{
}

BEGIN_MESSAGE_MAP(CWorldOutliner, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_CLASS_ADD_MEMBER_FUNCTION, OnClassAddMemberFunction)
	ON_COMMAND(ID_CLASS_ADD_MEMBER_VARIABLE, OnClassAddMemberVariable)
	ON_COMMAND(ID_CLASS_DEFINITION, OnClassDefinition)
	ON_COMMAND(ID_CLASS_PROPERTIES, OnClassProperties)
	ON_COMMAND(ID_NEW_FOLDER, OnNewFolder)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_COMMAND_RANGE(ID_SORTING_GROUPBYTYPE, ID_SORTING_SORTBYACCESS, OnSort)
	ON_UPDATE_COMMAND_UI_RANGE(ID_SORTING_GROUPBYTYPE, ID_SORTING_SORTBYACCESS, OnUpdateSort)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorldOutliner 메시지 처리기

int CWorldOutliner::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 뷰를 만듭니다.
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_HASBUTTONS | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;// | TVS_LINESATROOT;

	if (!m_wndClassView.Create(dwViewStyle, rectDummy, this, 2))
	{
		TRACE0("월드 아웃라이너를 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	// 이미지를 로드합니다.
	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_SORT);
	m_wndToolBar.LoadToolBar(IDR_SORT, 0, 0, TRUE /* 잠금 */);

	m_wndClassView.SetBkColor(RGB(32, 32, 32));
	m_wndClassView.SetLineColor(RGB(255, 255, 255));
	m_wndClassView.SetTextColor(RGB(255, 255, 255));

	OnChangeVisualStyle();

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_wndToolBar.SetOwner(this);

	// 모든 명령은 부모 프레임이 아닌 이 컨트롤을 통해 라우팅됩니다.
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	CMenu menuSort;
	menuSort.LoadMenu(IDR_POPUP_SORT);

	m_wndToolBar.ReplaceButton(ID_SORT_MENU, CClassViewMenuButton(menuSort.GetSubMenu(0)->GetSafeHmenu()));

	CClassViewMenuButton* pButton =  DYNAMIC_DOWNCAST(CClassViewMenuButton, m_wndToolBar.GetButton(0));

	if (pButton != nullptr)
	{
		pButton->m_bText = FALSE;
		pButton->m_bImage = TRUE;
		pButton->SetImage(GetCmdMgr()->GetCmdImage(m_nCurrSort));
		pButton->SetMessageWnd(this);
	}

	// 정적 트리 뷰 데이터를 더미 코드로 채웁니다.
	FillClassView();

	WorldCreatedDelegate.AddRow(this, &CWorldOutliner::OnWorldCreated);

	return 0;
}

void CWorldOutliner::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CWorldOutliner::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*)&m_wndClassView;
	ASSERT_VALID(pWndTree);

	if (pWnd != pWndTree)
	{
		CDockablePane::OnContextMenu(pWnd, point);
		return;
	}

	if (point != CPoint(-1, -1))
	{
		// 클릭한 항목을 선택합니다.
		CPoint ptTree = point;
		pWndTree->ScreenToClient(&ptTree);

		UINT flags = 0;
		HTREEITEM hTreeItem = pWndTree->HitTest(ptTree, &flags);
		if (hTreeItem != nullptr)
		{
			pWndTree->SelectItem(hTreeItem);
		}
	}

	pWndTree->SetFocus();
	CMenu menu;
	menu.LoadMenu(IDR_POPUP_SORT);

	CMenu* pSumMenu = menu.GetSubMenu(0);

	if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
	{
		CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

		if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
			return;

		((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
		UpdateDialogControls(this, FALSE);
	}
}

void CWorldOutliner::AdjustLayout()
{
	if (GetSafeHwnd() == nullptr)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(nullptr, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndClassView.SetWindowPos(nullptr, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

BOOL CWorldOutliner::PreTranslateMessage(MSG* pMsg)
{
	return CDockablePane::PreTranslateMessage(pMsg);
}

void CWorldOutliner::OnWorldCreated(UWorld* NewWorld)
{
	DeleteAllItems();
	InsertRootItem(NewWorld->GetName().c_str());
	OnSelectedItemChanged(hClassViewRoot);

	NewWorld->OnActorSpawned.AddRow(this, &CWorldOutliner::OnActorSpawned);
	NewWorld->WorldChangedDelegate.AddRow(this, &CWorldOutliner::OnWorldChanged);
}

void CWorldOutliner::OnWorldDestroyed(UWorld* NewWorld)
{
	//if (NewWorld->WorldType == EWorldType::PIE)
	//{
	//	if (GWorld->WorldType != EWorldType::Editor)
	//	{
	//		// PIE가 종료될때 GWorld가 Editor World로 바뀌어야 합니다
	//		_ASSERT(false);
	//		return;
	//	}

	//	DeleteAllItems();
	//	InsertRootItem(NewWorld->GetName().c_str());
	//	OnSelectedItemChanged(hClassViewRoot);
	//}
}

void CWorldOutliner::OnActorSpawned(AActor* NewActor)
{	
	const FString NewName = NewActor->GetName() + TEXT(" (") + NewActor->GetClass()->GetName() + TEXT(")");
	//// TODO Parent
	InsertItem(NewActor, NewName.c_str(), 1, 1, NULL);
}

void CWorldOutliner::OnWorldChanged(UWorld* NewWorld, TArray<TObjectPtr<AActor>>& NewActors)
{
	DeleteAllItems();
	InsertRootItem(NewWorld->GetName().c_str());
	OnSelectedItemChanged(hClassViewRoot);

	for (TEnginePtr<AActor> Actor : NewActors)
	{
		OnActorSpawned(Actor);
	}
}

void CWorldOutliner::OnSort(UINT id)
{
	if (m_nCurrSort == id)
	{
		return;
	}

	m_nCurrSort = id;

	CClassViewMenuButton* pButton =  DYNAMIC_DOWNCAST(CClassViewMenuButton, m_wndToolBar.GetButton(0));

	if (pButton != nullptr)
	{
		pButton->SetImage(GetCmdMgr()->GetCmdImage(id));
		m_wndToolBar.Invalidate();
		m_wndToolBar.UpdateWindow();
	}
}

void CWorldOutliner::OnUpdateSort(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(pCmdUI->m_nID == m_nCurrSort);
}

void CWorldOutliner::OnClassAddMemberFunction()
{
	AfxMessageBox(_T("멤버 함수 추가..."));
}

void CWorldOutliner::OnClassAddMemberVariable()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void CWorldOutliner::OnClassDefinition()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void CWorldOutliner::OnClassProperties()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void CWorldOutliner::OnNewFolder()
{
	AfxMessageBox(_T("새 폴더..."));
}

void CWorldOutliner::OnPaint()
{
	CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

	CRect rectTree;
	m_wndClassView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CWorldOutliner::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndClassView.SetFocus();
}

void CWorldOutliner::OnChangeVisualStyle()
{
	m_ClassViewImages.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_CLASS_VIEW_24 : IDB_CLASS_VIEW;

	CBitmap bmp;
	if (!bmp.LoadBitmap(uiBmpId))
	{
		TRACE(_T("비트맵을 로드할 수 없습니다. %x\n"), uiBmpId);
		ASSERT(FALSE);
		return;
	}

	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

	m_ClassViewImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_ClassViewImages.Add(&bmp, RGB(255, 0, 0));

	m_wndClassView.SetImageList(&m_ClassViewImages, TVSIL_NORMAL);

	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_SORT_24 : IDR_SORT, 0, 0, TRUE /* 잠금 */);
}

void CWorldOutliner::InsertRootItem(const wchar_t* NewName)
{
	hClassViewRoot = m_wndClassView.InsertItem(NewName, 0, 0);
	_ASSERT(hClassViewRoot);
}

void CWorldOutliner::InsertItem(AActor* ActorPointer, const wchar_t* NewName, int nImage, int nSelectedImage, _In_ HTREEITEM hParent)
{	
	if (hClassViewRoot == NULL)
	{
		_ASSERT(false); // no root!
	}

	if (hParent == NULL)
	{
		hParent = hClassViewRoot;
	}

	HTREEITEM NewItem = m_wndClassView.InsertItem(NewName, 1, 1, hParent);
	_ASSERT(NewItem);
	Actors.emplace(NewItem, ActorPointer);

	m_wndClassView.Expand(hClassViewRoot, TVE_EXPAND);
}

void CWorldOutliner::DeleteAllItems()
{
	m_wndClassView.DeleteAllItems();
	Actors.clear();
}

void CWorldOutliner::OnSelectedItemChanged(HTREEITEM NewSelectedItem)
{
	// Item을 WorldOutliner에서 선택한 경우 이쪽으로 선택한 아이템의 정보(HTREEITEM)
	// 가 들어옵니다.

	// 선택한 아이템이 RootNode인 경우 nullptr 전달
	if (NewSelectedItem == hClassViewRoot)
	{
		OnActorSelectedAndMakeDetails(nullptr);
		return;
	}

	// 선택한 Item으로 해당하는 Actor를 구합니다.
	auto It = Actors.find(NewSelectedItem);
	if (It == Actors.end())
	{
		// 검색 실패
		_ASSERT(false);
		return;
	}

	// 구한 Actor정보를 디테일 창에 표시합니다
	AActor* ActorAddress = It->second;
	OnActorSelectedAndMakeDetails(ActorAddress);
}

void CWorldOutliner::OnActorSelectedAndMakeDetails(AActor* SelectedActor)
{
	if (LastSelectedActor == SelectedActor) { return; }

	CMainFrame* MainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
	CMFCPropertyGridCtrl& Details = MainFrame->GetDetailPanel().GetPropList();
	
	bool bSameActor = true;
	if (LastSelectedActor != SelectedActor)
	{
		bSameActor = false;
		DetailsUI.clear();
		PropertyInfos.clear();
		Details.RemoveAll();
		LastSelectedActor = SelectedActor;
	}

	if (LastSelectedActor == nullptr)
	{
		Details.Invalidate();
		Details.UpdateWindow();
		return;
	}

	// 이름 중복에 유의!
	{
		{
			{
				CMFCPropertyGridProperty* Prop_ActorName = nullptr;
				static const FString Prop_ActorNameKey = TEXT("Prop_Actor_Name");
				if (bSameActor) { Prop_ActorName = DetailsUI.find(Prop_ActorNameKey)->second; }
				else
				{
					Prop_ActorName = new CMFCPropertyGridProperty(TEXT("Name"), SelectedActor->GetName().c_str());
					DetailsUI.emplace(Prop_ActorNameKey, Prop_ActorName);
					Details.AddProperty(Prop_ActorName);
				}
			}

			{
				CMFCPropertyGridProperty* Prop_ActorUProperty_Name = nullptr;
				static const FString Prop_ActorUPropertyNameKey = TEXT("Prop_ActorUProperty_Name");
				if (bSameActor) { Prop_ActorUProperty_Name = DetailsUI.find(Prop_ActorUPropertyNameKey)->second; }
				else
				{
					Prop_ActorUProperty_Name = new CMFCPropertyGridProperty(TEXT("Actor 속성"));
					DetailsUI.emplace(Prop_ActorUPropertyNameKey, Prop_ActorUProperty_Name);
				}
				
				FillDetails(bSameActor, Prop_ActorUProperty_Name, SelectedActor);

				Details.AddProperty(Prop_ActorUProperty_Name);
			}

		}

		// Components
		{
			vector<UActorComponent*> Components;
			SelectedActor->GetComponents<UActorComponent>(Components);
			for (UActorComponent* Component : Components)
			{
				// Component Start
				const FString ComponentName = Component->GetName() + TEXT(" (") + Component->GetClass()->GetName() + TEXT(")");
				CMFCPropertyGridProperty* Prop_Component = nullptr;
				if (bSameActor) { Prop_Component = DetailsUI.find(ComponentName)->second; }
				else
				{
					Prop_Component = new CMFCPropertyGridProperty(ComponentName.c_str());
					DetailsUI.emplace(ComponentName, Prop_Component);
				}

				// Component Reflection Data
				FillDetails(bSameActor, Prop_Component, Component);

				if (!bSameActor)
				{
					Details.AddProperty(Prop_Component);
				}
			}
		}
	}
}

extern CORE_API map<UClass*, multimap<FString, TEnginePtr<UObject>>> ObjectMap;
void CWorldOutliner::FillDetails(const bool bSameActor, CMFCPropertyGridProperty* ParentUI, UObject* InObject)
{
	type Type = resolve(Hash(InObject->GetClass()->ClassName.data()));
	Type.data([&](meta::data Data)
		{
			Data.prop([&](meta::prop p)
				{
					FProperty Property = p.value().cast<FProperty>();
					if (Property.PropertyFlags == EPropertyFlags::NoFlags)
					{
						return;
					}
					FString PropName = to_wstring(Property.Name);
					const FString Prop_Key = InObject->GetName() + TEXT("_") + PropName + TEXT("_Key");

					CMFCPropertyGridProperty* Prop_ActorUProperty = nullptr;
					if (bSameActor)
					{
						Prop_ActorUProperty = DetailsUI.find(Prop_Key)->second;
					}
					else
					{
						CMFCPropertyGridProperty* NewPropUI = nullptr;
						void* PropertyAddress = nullptr;
						switch (Property.PropertyType)
						{
							/*case T_ENGINE_PTR:
								break;*/
						case T_BOOL:
						{
							bool* Value = (bool*)Data.get(handle(Type.GetNode(), InObject)).data();
							NewPropUI = new CMFCPropertyGridProperty(PropName.data(), _variant_t(*Value), _T(""));
							PropertyAddress = (void*)Value;
							break;
						}
						/*case T_UINT8:
							break;*/
						case T_INT:
						{
							int* Value = (int*)Data.get(handle(Type.GetNode(), InObject)).data();
							NewPropUI = new CMFCPropertyGridProperty(PropName.data(), _variant_t(*Value), TEXT(""));
							PropertyAddress = (void*)Value;
							break;
						}
						case T_FLOAT:
						{
							float* Value = (float*)Data.get(handle(Type.GetNode(), InObject)).data();
							NewPropUI = new CMFCPropertyGridProperty(PropName.data(), _variant_t(*Value), TEXT(""));
							PropertyAddress = (void*)Value;
							break;
						}
						case T_FVECTOR:
						{
							FVector* Value = (FVector*)Data.get(handle(Type.GetNode(), InObject)).data();
							NewPropUI = new CMFCPropertyGridProperty(PropName.data());
							PropertyAddress = (void*)Value;

							CMFCPropertyGridProperty* XUI = new CMFCPropertyGridProperty(_T("X"), (_variant_t)Value->x, (PropName + _T(" X 값을 지정합니다.")).c_str(), (DWORD_PTR)&Value->x);
							CMFCPropertyGridProperty* YUI = new CMFCPropertyGridProperty(_T("Y"), (_variant_t)Value->y, (PropName + _T(" Y 값을 지정합니다.")).c_str(), (DWORD_PTR)&Value->y);
							CMFCPropertyGridProperty* ZUI = new CMFCPropertyGridProperty(_T("Z"), (_variant_t)Value->z, (PropName + _T(" Z 값을 지정합니다.")).c_str(), (DWORD_PTR)&Value->z);
							
							{
								FProperty PropertyInternal = Property;
								PropertyInternal.PropertyType = EPropertyType::T_FLOAT;
								PropertyInternal.PropertySize = 4;
								{
									auto ItX = PropertyInfos.emplace(XUI, FPropertyInfo(PropertyInternal, &Value->x));
									XUI->SetData((DWORD_PTR)&ItX.first->second);
									auto ItY = PropertyInfos.emplace(YUI, FPropertyInfo(PropertyInternal, &Value->y));
									YUI->SetData((DWORD_PTR)&ItY.first->second);
									auto ItZ = PropertyInfos.emplace(ZUI, FPropertyInfo(PropertyInternal, &Value->z));
									ZUI->SetData((DWORD_PTR)&ItZ.first->second);
								}
							}

							NewPropUI->AddSubItem(XUI);
							NewPropUI->AddSubItem(YUI);
							NewPropUI->AddSubItem(ZUI);
							break;
						}

						case T_FROTATOR:
						{
							FRotator* Value = (FRotator*)Data.get(handle(Type.GetNode(), InObject)).data();
							NewPropUI = new CMFCPropertyGridProperty(PropName.data());
							PropertyAddress = (void*)Value;

							CMFCPropertyGridProperty* RollUI = new CMFCPropertyGridProperty(_T("Roll"), (_variant_t)Value->Roll, (PropName + _T(" Roll 값을 지정합니다.")).c_str(), (DWORD_PTR)&Value->Roll);
							CMFCPropertyGridProperty* PitchUI = new CMFCPropertyGridProperty(_T("Pitch"), (_variant_t)Value->Pitch, (PropName + _T(" Pitch 값을 지정합니다.")).c_str(), (DWORD_PTR)&Value->Pitch);
							CMFCPropertyGridProperty* YawUI = new CMFCPropertyGridProperty(_T("Yaw"), (_variant_t)Value->Yaw, (PropName + _T(" Yaw 값을 지정합니다.")).c_str(), (DWORD_PTR)&Value->Yaw);

							{
								FProperty PropertyInternal = Property;
								PropertyInternal.PropertyType = EPropertyType::T_FLOAT;
								PropertyInternal.PropertySize = 4;
								{
									auto ItRoll = PropertyInfos.emplace(RollUI, FPropertyInfo(PropertyInternal, &Value->Roll));
									RollUI->SetData((DWORD_PTR)&ItRoll.first->second);
									auto ItPitch = PropertyInfos.emplace(PitchUI, FPropertyInfo(PropertyInternal, &Value->Pitch));
									PitchUI->SetData((DWORD_PTR)&ItPitch.first->second);
									auto ItYaw = PropertyInfos.emplace(YawUI, FPropertyInfo(PropertyInternal, &Value->Yaw));
									YawUI->SetData((DWORD_PTR)&ItYaw.first->second);
								}
							}

							NewPropUI->AddSubItem(RollUI);
							NewPropUI->AddSubItem(PitchUI);
							NewPropUI->AddSubItem(YawUI);
							break;
						}
						case T_ENGINE_PTR:
						{
							TEnginePtr<UObject>* Value = (TEnginePtr<UObject>*)Data.get(handle(Type.GetNode(), InObject)).data();
							TEnginePtr<UObject> EnginePtr = *Value;
							FString Name;
							if (EnginePtr)
							{
								Name = EnginePtr->GetName();
							}
							UClass* Class = UClass::FindClass(Property.ClassName);
							multimap<FString, TEnginePtr<UObject>> Objects = ObjectMap[Class];
							NewPropUI = new CMFCPropertyGridProperty(PropName.data(), Name.c_str(), TEXT(""));
							for (auto& It : Objects)
							{
								NewPropUI->AddOption(It.first.c_str(), TRUE);
							}
							NewPropUI->AllowEdit(FALSE);
							{
								PropertyAddress = Value;
							}
							break;
						}
						default:
							_ASSERT(false);
							break;
						}
						if (NewPropUI)
						{
							if (Property.PropertyFlags & EPropertyFlags::VisibleAnywhere) { NewPropUI->Enable(false); }
							DetailsUI.emplace(Prop_Key, NewPropUI);
							auto It = PropertyInfos.emplace(NewPropUI, FPropertyInfo(Property, PropertyAddress));
							NewPropUI->SetData((DWORD_PTR)&It.first->second);
							ParentUI->AddSubItem(NewPropUI);
						}
					}
				}
			);
		}
	);
}

void CWorldOutliner::FillClassView()
{
	//hClassViewRoot = m_wndWorldOutliner.InsertItem(_T("Map"), 0, 0);
	//m_wndWorldOutliner.SetItemState(hClassViewRoot, TVIS_BOLD, TVIS_BOLD);

	/*HTREEITEM hClass = m_wndWorldOutliner.InsertItem(_T("CFakeAboutDlg"), 1, 1);
	hClassViewRoot = hClass;
	m_wndWorldOutliner.InsertItem(_T("CFakeAboutDlg()"), 3, 3, hClass);

	m_wndWorldOutliner.Expand(hClassViewRoot, TVE_EXPAND);
	hClass = m_wndWorldOutliner.InsertItem(_T("CFakeApp"), 1, 1, hClassViewRoot);
	m_wndWorldOutliner.InsertItem(_T("CFakeApp()"), 3, 3, hClass);
	m_wndWorldOutliner.InsertItem(_T("InitInstance()"), 3, 3, hClass);
	m_wndWorldOutliner.InsertItem(_T("OnAppAbout()"), 3, 3, hClass);

	hClass = m_wndWorldOutliner.InsertItem(_T("CFakeAppDoc"), 1, 1, hClassViewRoot);
	m_wndWorldOutliner.InsertItem(_T("CFakeAppDoc()"), 4, 4, hClass);
	m_wndWorldOutliner.InsertItem(_T("~CFakeAppDoc()"), 3, 3, hClass);
	m_wndWorldOutliner.InsertItem(_T("OnNewDocument()"), 3, 3, hClass);

	hClass = m_wndWorldOutliner.InsertItem(_T("CFakeAppView"), 1, 1, hClassViewRoot);
	m_wndWorldOutliner.InsertItem(_T("CFakeAppView()"), 4, 4, hClass);
	m_wndWorldOutliner.InsertItem(_T("~CFakeAppView()"), 3, 3, hClass);
	m_wndWorldOutliner.InsertItem(_T("GetDocument()"), 3, 3, hClass);
	m_wndWorldOutliner.Expand(hClass, TVE_EXPAND);

	hClass = m_wndWorldOutliner.InsertItem(_T("CFakeAppFrame"), 1, 1, hClassViewRoot);
	m_wndWorldOutliner.InsertItem(_T("CFakeAppFrame()"), 3, 3, hClass);
	m_wndWorldOutliner.InsertItem(_T("~CFakeAppFrame()"), 3, 3, hClass);
	m_wndWorldOutliner.InsertItem(_T("m_wndMenuBar"), 6, 6, hClass);
	m_wndWorldOutliner.InsertItem(_T("m_wndToolBar"), 6, 6, hClass);
	m_wndWorldOutliner.InsertItem(_T("m_wndStatusBar"), 6, 6, hClass);

	hClass = m_wndWorldOutliner.InsertItem(_T("Globals"), 2, 2, hClassViewRoot);
	m_wndWorldOutliner.InsertItem(_T("theFakeApp"), 5, 5, hClass);
	m_wndWorldOutliner.Expand(hClass, TVE_EXPAND);*/
}

BEGIN_MESSAGE_MAP(CMyViewTree, CViewTree)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, &CMyViewTree::OnTvnSelchanged)
END_MESSAGE_MAP()


void CMyViewTree::OnTvnSelchanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	// 선택된 항목의 핸들을 가져옴 
	HTREEITEM hSelectedItem = pNMTreeView->itemNew.hItem; 
	// 선택된 항목의 텍스트를 가져옴
	//CString strItemText = GetItemText(hSelectedItem); 

	CWorldOutliner* WorldOutliner = static_cast<CWorldOutliner*>(GetParent());
	WorldOutliner->OnSelectedItemChanged(hSelectedItem);

	*pResult = 0;
}


#endif // #if WITH_EDITOR

