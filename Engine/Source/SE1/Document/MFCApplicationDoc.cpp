
// MFCApplicationDoc.cpp: CMFCApplicationDoc 클래스의 구현
//
#include "Misc/MFCHeaders.h"
#include "Application.h"
#include "Misc/MFCHeaders.h"
#include "MFCApplicationDoc.h"
#include "Misc/MFCHeaders.h"
#if WITH_EDITOR
#include "EngineMinimal.h"
#include <propkey.h>

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif

// CMFCApplicationDoc

IMPLEMENT_DYNCREATE(CMFCApplicationDoc, CDocument)

BEGIN_MESSAGE_MAP(CMFCApplicationDoc, CDocument)
END_MESSAGE_MAP()


// CMFCApplicationDoc 생성/소멸

CMFCApplicationDoc::CMFCApplicationDoc() noexcept
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.

}

CMFCApplicationDoc::~CMFCApplicationDoc()
{
}

BOOL CMFCApplicationDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 재초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.

	return TRUE;
}




// CMFCApplicationDoc serialization

void CMFCApplicationDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 여기에 저장 코드를 추가합니다.
		string SaveString = GEngine->Save();
		CString Str = SaveString.c_str();
		ar << Str;
	}
	else
	{
		// TODO: 여기에 로딩 코드를 추가합니다.
		CString Str;
		ar >> Str;
		FString WString = Str.GetString();
		string LoadedString = TCHAR_TO_ANSI(WString);
		GEngine->Load(LoadedString);
	}
}

// CMFCApplicationDoc 진단

#ifdef _DEBUG
void CMFCApplicationDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMFCApplicationDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CMFCApplicationDoc 명령
#endif // #if WITH_EDITOR