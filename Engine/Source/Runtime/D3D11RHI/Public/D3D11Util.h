#pragma once
#include "CoreMinimal.h"

class ID3D11Device;
class ID3D11DeviceContext;
class ID3D11RenderTargetView;
class ID3D11DepthStencilView;

/*=============================================================================
	D3D11Util.h: D3D RHI utility definitions.
=============================================================================*/

#define D3D11RHI_IMMEDIATE_CONTEXT	(GD3D11RHI->GetDeviceContext())
#define D3D11RHI_DEVICE				(GD3D11RHI->GetDevice())

/**
 * 결과가 실패가 아닌지 확인합니다. 실패한 경우 애플리케이션은 종료되지 않고 적절한 오류 메시지만 로그합니다.
 * @param   Result - 확인할 결과 코드입니다.
 * @param   Code - 결과를 생성한 코드입니다.
 * @param   Filename - Code를 포함하는 소스 파일의 파일 이름입니다.
 * @param   Line - Filename 내에서 Code의 줄 번호입니다.
 */
extern D3D11RHI_API void VerifyD3D11ResultNoExit(HRESULT Result, const ANSICHAR* Code, const ANSICHAR* Filename, uint32 Line, ID3D11Device* Device);

/**
 * 결과가 실패가 아닌지 확인합니다. 실패한 경우 애플리케이션은 적절한 오류 메시지와 함께 종료됩니다.
 * @param   Result - 확인할 결과 코드입니다.
 * @param   Code - 결과를 생성한 코드입니다.
 * @param   Filename - Code를 포함하는 소스 파일의 파일 이름입니다.
 * @param   Line - Filename 내에서 Code의 줄 번호입니다.
 */
extern D3D11RHI_API void VerifyD3D11Result(HRESULT Result, const ANSICHAR* Code, const ANSICHAR* Filename, uint32 Line, ID3D11Device* Device);

/**
 * 주어진 결과가 실패가 아닌지 확인합니다. 실패인 경우, 적절한 오류 메시지와 함께 애플리케이션이 종료됩니다.
 * @param   Shader - 생성하려는 셰이더.
 * @param   Result - 확인할 결과 코드.
 * @param   Code - 해당 결과를 도출한 코드.
 * @param   Filename - Code를 포함하는 소스 파일의 파일명.
 * @param   Line - Filename 내에서 Code의 위치한 행 번호.
 * @param   Device - 셰이더를 생성하는 데 사용된 D3D 디바이스.
 */
extern D3D11RHI_API void VerifyD3D11ShaderResult(class FRHIShader* Shader, HRESULT Result, const ANSICHAR* Code, const ANSICHAR* Filename, uint32 Line, ID3D11Device* Device);

/**
* 주어진 결과가 실패가 아닌지 확인합니다. 실패일 경우, 해당 오류 메시지와 함께 애플리케이션이 종료됩니다.
* @param	Result - 확인할 결과 코드
* @param	Code - 결과를 생성한 코드
* @param	Filename - Code를 포함하는 소스 파일의 파일 이름
* @param	Line - Filename 내의 Code의 라인 번호
*/
extern D3D11RHI_API void VerifyD3D11CreateTextureResult(HRESULT D3DResult, int32 UEFormat, const ANSICHAR* Code, const ANSICHAR* Filename, uint32 Line,
    uint32 SizeX, uint32 SizeY, uint32 SizeZ, uint8 D3DFormat, uint32 NumMips, uint32 Flags, D3D11_USAGE Usage,
    uint32 CPUAccessFlags, uint32 MiscFlags, uint32 SampleCount, uint32 SampleQuality,
    const void* SubResPtr, uint32 SubResPitch, uint32 SubResSlicePitch, ID3D11Device* Device, const TCHAR* DebugName);


/**
 * A macro for using VERIFYD3D11RESULT that automatically passes in the code and filename/line.
 */
#define VERIFYD3D11RESULT_EX(x, Device)	{HRESULT hr = x; if (FAILED(hr)) { VerifyD3D11Result(hr,#x,__FILE__,__LINE__, Device); }}
#define VERIFYD3D11RESULT(x)			{HRESULT hr = x; if (FAILED(hr)) { VerifyD3D11Result(hr,#x,__FILE__,__LINE__, 0); }}
#define VERIFYD3D11RESULT_NOEXIT(x)		{HRESULT hr = x; if (FAILED(hr)) { VerifyD3D11ResultNoExit(hr,#x,__FILE__,__LINE__, 0); }}
#define VERIFYD3D11SHADERRESULT(Result, Shader, Device) {HRESULT hr = (Result); if (FAILED(hr)) { VerifyD3D11ShaderResult(Shader, hr, #Result,__FILE__,__LINE__, Device); }}
#define VERIFYD3D11RESULT_NOEXIT(x)		{HRESULT hr = x; if (FAILED(hr)) { VerifyD3D11ResultNoExit(hr,#x,__FILE__,__LINE__, 0); }}
#define VERIFYD3D11CREATETEXTURERESULT(x,UEFormat,SizeX,SizeY,SizeZ,Format,NumMips,Flags,Usage,CPUAccessFlags,MiscFlags,SampleCount,SampleQuality,SubResPtr,SubResPitch,SubResSlicePitch,Device,DebugName) {HRESULT hr = x; if (FAILED(hr)) { VerifyD3D11CreateTextureResult(hr, UEFormat,#x,__FILE__,__LINE__,SizeX,SizeY,SizeZ,Format,NumMips,Flags,Usage,CPUAccessFlags,MiscFlags,SampleCount,SampleQuality,SubResPtr,SubResPitch,SubResSlicePitch,Device,DebugName); }}
//#define VERIFYD3D11RESIZEVIEWPORTRESULT(x, OldState, NewState, Device) { HRESULT hr = x; if (FAILED(hr)) { VerifyD3D11ResizeViewportResult(hr, #x, __FILE__, __LINE__, OldState, NewState, Device); }}
//#define VERIFYD3D11CREATEVIEWRESULT(x, Device, Resource, Desc) {HRESULT hr = x; if (FAILED(hr)) { VerifyD3D11CreateViewResult(hr, #x, __FILE__, __LINE__, Device, Resource, Desc); }}

/** 제공된 오류 코드에 대한 문자열을 반환하며, 디바이스가 제공된 경우 디바이스 제거 정보도 포함할 수 있습니다. */
FString GetD3D11ErrorString(HRESULT ErrorCode, ID3D11Device* Device);

/**
 * 디바이스 컨텍스트에 현재 바인딩된 렌더 타겟을 검색하는 클래스.
 */
class FD3D11BoundRenderTargets
{
public:
    /** 초기화 생성자: 디바이스 컨텍스트가 필요합니다. */
    explicit FD3D11BoundRenderTargets(ID3D11DeviceContext* InDeviceContext);

    /** 소멸자. */
    ~FD3D11BoundRenderTargets();

    /** 접근자. */
    FORCEINLINE int32 GetNumActiveTargets() const { return NumActiveTargets; }
    FORCEINLINE ID3D11RenderTargetView* GetRenderTargetView(int32 TargetIndex) { return RenderTargetViews[TargetIndex]; }
    FORCEINLINE ID3D11DepthStencilView* GetDepthStencilView() { return DepthStencilView; }

private:
    /** 활성 렌더 타겟 뷰. */
    ID3D11RenderTargetView* RenderTargetViews[MaxSimultaneousRenderTargets];
    /** 활성 깊이 스텐실 뷰. */
    ID3D11DepthStencilView* DepthStencilView;
    /** 활성 렌더 타겟의 수. */
    int32 NumActiveTargets;
};
