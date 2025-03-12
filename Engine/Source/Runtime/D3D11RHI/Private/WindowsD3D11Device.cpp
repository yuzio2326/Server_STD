#include "D3D11RHIPrivate.h"
#include "RenderResource.h"
#include "RHICommandList.h"

FD3D11DynamicRHI* GD3D11RHI = nullptr;
bool GIsDebugLayerEnabled = false;

/**
 * CreateDXGIFactory1이 D3D11 DLL에서 지연 로드되는 가져오기이기 때문에, 사용자가 VistaSP2/DX10을 사용하지 않는 경우
 * CreateDXGIFactory1을 호출하면 예외가 발생합니다.
 * 우리는 SEH를 사용하여 그 경우를 감지하고 우아하게 실패 처리합니다.
 */
static void SafeCreateDXGIFactory(IDXGIFactory1** DXGIFactory1, bool bWithDebug)
{
	// CreateDXGIFactory2는 Win8.1+에서만 사용할 수 있으며, 존재하는 경우 찾습니다.
	uint32 Flags = bWithDebug ? DXGI_CREATE_FACTORY_DEBUG : 0;
	// 우리는 무조건 아래 기능을 지원한다고 보고 작업 한다.
	CreateDXGIFactory2(Flags, __uuidof(IDXGIFactory2), (void**)DXGIFactory1);
}

/**
 * 명령줄 매개변수에 기반하여 생성할 수 있는 가장 낮은 D3D 기능 수준을 반환합니다.
 */
static D3D_FEATURE_LEVEL GetMinAllowedD3DFeatureLevel()
{
	return D3D_FEATURE_LEVEL_11_0;
}

/**
 * 명령줄 매개변수에 기반하여 생성할 수 있는 가장 높은 D3D 기능 수준을 반환합니다.
 */
static D3D_FEATURE_LEVEL GetMaxAllowedD3DFeatureLevel()
{
	return D3D_FEATURE_LEVEL_11_1;
}

const TCHAR* GetFeatureLevelString(D3D_FEATURE_LEVEL FeatureLevel)
{
	switch (FeatureLevel)
	{
	case D3D_FEATURE_LEVEL_9_1:		return TEXT("9_1");
	case D3D_FEATURE_LEVEL_9_2:		return TEXT("9_2");
	case D3D_FEATURE_LEVEL_9_3:		return TEXT("9_3");
	case D3D_FEATURE_LEVEL_10_0:	return TEXT("10_0");
	case D3D_FEATURE_LEVEL_10_1:	return TEXT("10_1");
	case D3D_FEATURE_LEVEL_11_0:	return TEXT("11_0");
	case D3D_FEATURE_LEVEL_11_1:	return TEXT("11_1");
	}
	return TEXT("X_X");
}

static void LogDXGIAdapterDesc(const DXGI_ADAPTER_DESC& AdapterDesc)
{
	E_LOG(Log, TEXT("    Description : {}"), AdapterDesc.Description);
	E_LOG(Log, TEXT("    VendorId    : {:#04x}"), AdapterDesc.VendorId);
	E_LOG(Log, TEXT("    DeviceId    : {:#04x}"), AdapterDesc.DeviceId);
	E_LOG(Log, TEXT("    SubSysId    : {:#04x}"), AdapterDesc.SubSysId);
	E_LOG(Log, TEXT("    Revision    : {:#04x}"), AdapterDesc.Revision);
	E_LOG(Log, TEXT("    DedicatedVideoMemory : {} bytes"), AdapterDesc.DedicatedVideoMemory);
	E_LOG(Log, TEXT("    DedicatedSystemMemory : {} bytes"), AdapterDesc.DedicatedSystemMemory);
	E_LOG(Log, TEXT("    SharedSystemMemory : {} bytes"), AdapterDesc.SharedSystemMemory);
	E_LOG(Log, TEXT("    AdapterLuid : {} {}"), AdapterDesc.AdapterLuid.HighPart, AdapterDesc.AdapterLuid.LowPart);
}


/**
 * 최대 MaxFeatureLevel을 사용하여 어댑터에 대한 D3D11 장치를 생성하려고 시도합니다.
 * 생성이 성공하면 true를 반환하고 지원되는 기능 수준이 OutFeatureLevel에 설정됩니다.
 */
static bool SafeTestD3D11CreateDevice(IDXGIAdapter* Adapter, D3D_FEATURE_LEVEL MinFeatureLevel, D3D_FEATURE_LEVEL MaxFeatureLevel, D3D_FEATURE_LEVEL* OutFeatureLevel)
{
	ID3D11Device* D3DDevice = nullptr;
	ID3D11DeviceContext* D3DDeviceContext = nullptr;
	uint32 DeviceFlags = D3D11_CREATE_DEVICE_SINGLETHREADED;
	// Use a debug device if specified on the command line.
	if (GIsDebugLayerEnabled)//(GRHIGlobals.IsDebugLayerEnabled)
	{
		DeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	}

	//// @MIXEDREALITY_CHANGE : BEGIN - Add BGRA flag for Windows Mixed Reality HMD's
	//DeviceFlags |= D3D11_CREATE_DEVICE_BGRA_SUPPORT;
	//// @MIXEDREALITY_CHANGE : END

	D3D_FEATURE_LEVEL RequestedFeatureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	// Trim to allowed feature levels
	int32 FirstAllowedFeatureLevel = 0;
	int32 NumAllowedFeatureLevels = ARRAYSIZE(RequestedFeatureLevels);
	int32 LastAllowedFeatureLevel = NumAllowedFeatureLevels - 1;

	while (FirstAllowedFeatureLevel < NumAllowedFeatureLevels)
	{
		if (RequestedFeatureLevels[FirstAllowedFeatureLevel] == MaxFeatureLevel)
		{
			break;
		}
		FirstAllowedFeatureLevel++;
	}

	while (LastAllowedFeatureLevel > 0)
	{
		if (RequestedFeatureLevels[LastAllowedFeatureLevel] >= MinFeatureLevel)
		{
			break;
		}
		LastAllowedFeatureLevel--;
	}

	NumAllowedFeatureLevels = LastAllowedFeatureLevel - FirstAllowedFeatureLevel + 1;
	if (MaxFeatureLevel < MinFeatureLevel || NumAllowedFeatureLevels <= 0)
	{
		return false;
	}

	try
	{
		// 소프트웨어 렌더러를 원하지 않습니다. 이상적으로는 생성 시 D3D_DRIVER_TYPE_HARDWARE를 지정하지만,
		// 어댑터를 지정할 때는 D3D_DRIVER_TYPE_UNKNOWN을 지정해야 합니다 (그렇지 않으면 호출이 실패합니다).
		// 나중에 장치 유형을 확인할 수 없습니다 (D3D에서 이 기능이 누락된 것 같습니다).
		HRESULT Result = D3D11CreateDevice(
			Adapter,
			D3D_DRIVER_TYPE_UNKNOWN,
			nullptr,
			DeviceFlags,
			&RequestedFeatureLevels[FirstAllowedFeatureLevel],
			NumAllowedFeatureLevels,
			D3D11_SDK_VERSION,
			&D3DDevice,
			OutFeatureLevel,
			&D3DDeviceContext);

		if (SUCCEEDED(Result))
		{
			D3DDevice->Release();
			D3DDeviceContext->Release();
			return true;
		}

		// 테스트 장치 생성 실패의 이유를 로그로 기록합니다. 추가 디버그 도움이 됩니다.
		VERIFYD3D11RESULT_NOEXIT(Result);

		//bool bIsWin10 = FPlatformMisc::VerifyWindowsVersion(10, 0);

		//// Fatal error on 0x887A002D
		//if (DXGI_ERROR_SDK_COMPONENT_MISSING == Result && bIsWin10)
		//{
		//	E_LOG(LogD3D11RHI, Fatal, TEXT("-d3ddebug was used but optional Graphics Tools were not found. Install them through the Manage Optional Features in windows. See: https://docs.microsoft.com/en-us/windows/uwp/gaming/use-the-directx-runtime-and-visual-studio-graphics-diagnostic-features"));
		//}
	}
	catch (...)
	{
		E_LOG(Error, TEXT("check"));
		// We suppress warning C6322: Empty _except block. Appropriate checks are made upon returning. 
		//CA_SUPPRESS(6322);
	}

	return false;
}

static uint32 CountAdapterOutputs(TRefCountPtr<IDXGIAdapter>& Adapter)
{
	uint32 OutputCount = 0;
	for (;;)
	{
		TRefCountPtr<IDXGIOutput> Output;
		HRESULT hr = Adapter->EnumOutputs(OutputCount, Output.GetInitReference());
		if (FAILED(hr))
		{
			break;
		}
		++OutputCount;
	}

	return OutputCount;
}


static bool CheckD3D11StoredMessages()
{
	bool bResult = false;

	TRefCountPtr<ID3D11Debug> DebugDevice = nullptr;
	VERIFYD3D11RESULT_EX(GD3D11RHI->GetDevice()->QueryInterface(__uuidof(ID3D11Debug), (void**)DebugDevice.GetInitReference()), GD3D11RHI->GetDevice());
	if (DebugDevice)
	{
		TRefCountPtr<ID3D11InfoQueue> d3dInfoQueue;
		if (SUCCEEDED(GD3D11RHI->GetDevice()->QueryInterface(__uuidof(ID3D11InfoQueue), (void**)d3dInfoQueue.GetInitReference())))
		{
			D3D11_MESSAGE* d3dMessage = nullptr;
			SIZE_T AllocateSize = 0;

			static bool bBreakOnWarning = false;// FParse::Param(FCommandLine::Get(), TEXT("d3dbreakonwarning"));

			int StoredMessageCount = (int)d3dInfoQueue->GetNumStoredMessagesAllowedByRetrievalFilter();
			for (int MessageIndex = 0; MessageIndex < StoredMessageCount; MessageIndex++)
			{
				SIZE_T MessageLength = 0;
				HRESULT hr = d3dInfoQueue->GetMessage(MessageIndex, nullptr, &MessageLength);

				// realloc the message
				if (MessageLength > AllocateSize)
				{
					if (d3dMessage)
					{
						std::free(d3dMessage);
						d3dMessage = nullptr;
						AllocateSize = 0;
					}

					d3dMessage = (D3D11_MESSAGE*)std::malloc(MessageLength);
					AllocateSize = MessageLength;
				}

				if (d3dMessage)
				{
					// get the actual message data from the queue
					hr = d3dInfoQueue->GetMessage(MessageIndex, d3dMessage, &MessageLength);

					switch (d3dMessage->Severity)
					{
					case D3D11_MESSAGE_SEVERITY_CORRUPTION:
					case D3D11_MESSAGE_SEVERITY_ERROR:
					{
						E_LOG(Error, TEXT("[D3DDebug] {}"), ANSI_TO_TCHAR(d3dMessage->pDescription));
						bResult = true;
						break;
					}
					case D3D11_MESSAGE_SEVERITY_WARNING:
					{
						E_LOG(Warning, TEXT("[D3DDebug] {}"), ANSI_TO_TCHAR(d3dMessage->pDescription));
						if (bBreakOnWarning)
						{
							bResult = true;
						}
						break;
					}
					default:
					{
						E_LOG(Log, TEXT("[D3DDebug] {}"), ANSI_TO_TCHAR(d3dMessage->pDescription));
					}
					}
				}
			}
			d3dInfoQueue->ClearStoredMessages();
			if (AllocateSize > 0)
			{
				std::free(d3dMessage);
			}
		}
	}

	return bResult;
}


static LONG __stdcall D3D11VectoredExceptionHandler(EXCEPTION_POINTERS* InInfo)
{
	// Only handle D3D error codes here
	if (InInfo->ExceptionRecord->ExceptionCode == _FACDXGI)
	{
		if (CheckD3D11StoredMessages())
		{
			if (IsDebuggerPresent())
			{
				// 여기에 도달하면, 이 오류 메시지에 대해 BreakOnSeverity가 설정되었음을 의미하므로, 
				// 디버거가 연결된 경우 여기도 디버그 브레이크를 요청합니다.
				//UE_DEBUG_BREAK();
				_ASSERT(false);
			}
		}

		// Handles the exception
		return EXCEPTION_CONTINUE_EXECUTION;
	}

	// continue searching
	return EXCEPTION_CONTINUE_SEARCH;
}


void FD3D11DynamicRHIModule::StartupModule()
{
}

bool FD3D11DynamicRHIModule::IsSupported()
{
	// if not computed yet
	if (!ChosenAdapter.IsValid())
	{
		FindAdapter();
	}

	// The hardware must support at least 11_0.
	return ChosenAdapter.IsValid()
		&& ChosenAdapter.MaxSupportedFeatureLevel >= D3D_FEATURE_LEVEL_11_0;
}

FDynamicRHI* FD3D11DynamicRHIModule::CreateRHI(ERHIFeatureLevel::Type RequestedFeatureLevel)
{
	IDXGIFactory1* DXGIFactory1;
	VERIFYD3D11RESULT(ChosenAdapter.DXGIAdapter->GetParent(__uuidof(DXGIFactory1), reinterpret_cast<void**>(&DXGIFactory1)));

	GD3D11RHI = new FD3D11DynamicRHI(DXGIFactory1, ChosenAdapter.MaxSupportedFeatureLevel, ChosenAdapter);
	return GD3D11RHI;
}

void FD3D11DynamicRHIModule::FindAdapter()
{
	FConfigFile& ConfigFile = GConfig->GetConfig(GEngineIni);
	ConfigFile.Get("/Script/WindowsTargetPlatform.WindowsTargetSettings", "GIsDebugLayerEnabled", GIsDebugLayerEnabled);

	// 일단 하나를 선택하면 다시 할 필요가 없습니다.
	_ASSERT(!ChosenAdapter.IsValid());

	// DXGIFactory1을 생성하려고 시도합니다. Vista SP2 이상을 실행하지 않는 경우 실패합니다.
	TRefCountPtr<IDXGIFactory1> DXGIFactory1;
	SafeCreateDXGIFactory(DXGIFactory1.GetInitReference(), GIsDebugLayerEnabled);
	if (!DXGIFactory1)
	{
		return;
	}

	TRefCountPtr<IDXGIFactory6> DXGIFactory6;
	DXGIFactory1->QueryInterface(__uuidof(IDXGIFactory6), (void**)DXGIFactory6.GetInitReference());


	// 우리는 Perf 안쓸꺼
	bool bAllowPerfHUD = false;// true;
	//#if UE_BUILD_SHIPPING || UE_BUILD_TEST
	//	bAllowPerfHUD = false;
	//#endif

	TRefCountPtr<IDXGIAdapter> TempAdapter;
	D3D_FEATURE_LEVEL MinAllowedFeatureLevel = GetMinAllowedD3DFeatureLevel();
	D3D_FEATURE_LEVEL MaxAllowedFeatureLevel = GetMaxAllowedD3DFeatureLevel();

	E_LOG(Log, TEXT("D3D11 min allowed feature level: {}"), GetFeatureLevelString(MinAllowedFeatureLevel));
	E_LOG(Log, TEXT("D3D11 max allowed feature level: {}"), GetFeatureLevelString(MaxAllowedFeatureLevel));

	FD3D11Adapter PreferredAdapter;
	FD3D11Adapter BestMemoryAdapter;
	FD3D11Adapter FirstDiscreteAdapter;
	FD3D11Adapter FirstAdapter;
	SIZE_T BestDedicatedMemory = 0;

	E_LOG(Log, TEXT("D3D11 adapters:"));

	const EGpuVendorId PreferredVendor = RHIGetPreferredAdapterVendor();
	bool bAllowSoftwareFallback = false;// D3D11RHI_AllowSoftwareFallback();


	int GpuPreferenceInt = DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE;
	// FParse::Value(FCommandLine::Get(), TEXT("-gpupreference="), GpuPreferenceInt);
	DXGI_GPU_PREFERENCE GpuPreference;
	switch (GpuPreferenceInt)
	{
	case 1: GpuPreference = DXGI_GPU_PREFERENCE_MINIMUM_POWER; break;
	case 2: GpuPreference = DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE; break;
	default: GpuPreference = DXGI_GPU_PREFERENCE_UNSPECIFIED; break;
	}

	auto LocalEnumAdapters = [&DXGIFactory6, &DXGIFactory1, GpuPreference](UINT AdapterIndex, IDXGIAdapter** Adapter) -> HRESULT
		{
			if (!DXGIFactory6 || GpuPreference == DXGI_GPU_PREFERENCE_UNSPECIFIED)
			{
				return DXGIFactory1->EnumAdapters(AdapterIndex, Adapter);
			}
			else
			{
				return DXGIFactory6->EnumAdapterByGpuPreference(AdapterIndex, GpuPreference, __uuidof(IDXGIAdapter), (void**)Adapter);
			}
		};

	// DXGIFactory의 어댑터를 열거합니다.
	for (uint32 AdapterIndex = 0; LocalEnumAdapters(AdapterIndex, TempAdapter.GetInitReference()) != DXGI_ERROR_NOT_FOUND; ++AdapterIndex)
	{
		// Check that if adapter supports D3D11.
		if (TempAdapter)
		{
			E_LOG(Log, TEXT("Testing D3D11 Adapter {}:"), AdapterIndex);
			DXGI_ADAPTER_DESC AdapterDesc;
			if (HRESULT DescResult = TempAdapter->GetDesc(&AdapterDesc); FAILED(DescResult))
			{
				E_LOG(Warning, TEXT("Failed to get description for adapter {}."), AdapterIndex);
			}
			else
			{
				LogDXGIAdapterDesc(AdapterDesc);
			}

			D3D_FEATURE_LEVEL ActualFeatureLevel = (D3D_FEATURE_LEVEL)0;
			if (SafeTestD3D11CreateDevice(TempAdapter, MinAllowedFeatureLevel, MaxAllowedFeatureLevel, &ActualFeatureLevel))
			{
				// 사용 가능한 D3D11 어댑터에 대한 정보를 로그로 기록합니다.

				uint32 OutputCount = CountAdapterOutputs(TempAdapter);

				E_LOG(Log,
					TEXT("  {}. '{}' (Feature Level {})"),
					AdapterIndex,
					AdapterDesc.Description,
					GetFeatureLevelString(ActualFeatureLevel)
				);
				E_LOG(Log,
					TEXT("      {}/{}/{} MB DedicatedVideo/DedicatedSystem/SharedSystem, Outputs:{}, VendorId:{:#x}"),
					(uint32)(AdapterDesc.DedicatedVideoMemory / (1024 * 1024)),
					(uint32)(AdapterDesc.DedicatedSystemMemory / (1024 * 1024)),
					(uint32)(AdapterDesc.SharedSystemMemory / (1024 * 1024)),
					OutputCount,
					AdapterDesc.VendorId
				);

				// Windows Advanced Rasterization Platform(WARP)과 관련된 변수를 나타냅니다. 
				// WARP는 하드웨어 가속이 없는 시스템에서도 DirectX 기능을 소프트웨어 렌더링을 통해 지원하는 기술입니다.
				const bool bIsWARP = RHIConvertToGpuVendorId(AdapterDesc.VendorId) == EGpuVendorId::Microsoft;

				// 간단한 휴리스틱이지만 프로파일링 없이 더 나은 결과를 얻기 어렵습니다.
				bool bIsNonLocalMemoryPresent = false;
				TRefCountPtr<IDXGIAdapter3> TempDxgiAdapter3;
				DXGI_QUERY_VIDEO_MEMORY_INFO NonLocalVideoMemoryInfo;
				if (SUCCEEDED(TempAdapter->QueryInterface(_uuidof(IDXGIAdapter3), (void**)TempDxgiAdapter3.GetInitReference())) &&
					TempDxgiAdapter3.IsValid() && SUCCEEDED(TempDxgiAdapter3->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_NON_LOCAL, &NonLocalVideoMemoryInfo)))
				{
					bIsNonLocalMemoryPresent = NonLocalVideoMemoryInfo.Budget != 0;
				}

				// TODO: GPUDetect를 사용하여 Intel GPU의 통합 여부와 개별 여부를 확인하는 작업을 GPUDetect 업데이트 대기 중에 수행합니다.

				const bool bIsIntegrated = !bIsNonLocalMemoryPresent;
				// PerfHUD는 성능 프로파일링을 위한 것입니다.
				const bool bIsPerfHUD = !std::wcscmp(AdapterDesc.Description, TEXT("NVIDIA PerfHUD"));

				FD3D11Adapter CurrentAdapter(TempAdapter, ActualFeatureLevel, bIsWARP, bIsIntegrated);

				// HMD(헤드 마운트 디스플레이)를 지원하기 위해 특별한 검사를 추가합니다. HMD는 관련 출력 장치가 없습니다.
				// 소프트웨어 에뮬레이션을 거부합니다, 단 cvar가 이를 원하지 않는 경우를 제외합니다.
				// https://msdn.microsoft.com/en-us/library/windows/desktop/bb205075(v=vs.85).aspx#WARP_new_for_Win8
				// 이전에는 출력 장치가 없는 경우를 테스트했으나, Intel(출력 있음)과 NVidia(출력 없음) GPU를 가진 노트북에서는 실패했습니다.
				const bool bSkipSoftwareAdapter = bIsWARP && !bAllowSoftwareFallback /*&& CVarExplicitAdapterValue < 0 && HmdGraphicsAdapterLuid == 0*/;

				// PerfHUD 어댑터를 허용하지 않습니다.
				const bool bSkipPerfHUDAdapter = bIsPerfHUD && !bAllowPerfHUD;

				// HMD는 이 어댑터가 아닌 특정 어댑터를 원합니다.
				//const bool bSkipHmdGraphicsAdapter = HmdGraphicsAdapterLuid != 0 && FMemory::Memcmp(&HmdGraphicsAdapterLuid, &AdapterDesc.AdapterLuid, sizeof(LUID)) != 0;

				// 사용자는 이 어댑터가 아닌 특정 어댑터를 원합니다.
				//const bool bSkipExplicitAdapter = CVarExplicitAdapterValue >= 0 && AdapterIndex != CVarExplicitAdapterValue;

				const bool bSkipAdapter = bSkipSoftwareAdapter || bSkipPerfHUDAdapter /*|| bSkipHmdGraphicsAdapter || bSkipExplicitAdapter*/;

				if (!bSkipAdapter && CurrentAdapter.IsValid())
				{
					if (PreferredVendor != EGpuVendorId::Unknown && PreferredVendor == RHIConvertToGpuVendorId(AdapterDesc.VendorId) && !PreferredAdapter.IsValid())
					{
						PreferredAdapter = CurrentAdapter;
					}

					if (!bIsWARP && !CurrentAdapter.bIsIntegrated)
					{
						if (!FirstDiscreteAdapter.IsValid())
						{
							FirstDiscreteAdapter = CurrentAdapter;
						}

						if (AdapterDesc.DedicatedVideoMemory > BestDedicatedMemory)
						{
							BestMemoryAdapter = CurrentAdapter;
							BestDedicatedMemory = AdapterDesc.DedicatedVideoMemory;
							if (PreferredVendor != EGpuVendorId::Unknown && PreferredVendor == RHIConvertToGpuVendorId(AdapterDesc.VendorId))
							{
								// 선호하는 IHV 장치 중에서 최적의 옵션을 선택하세요.
								PreferredAdapter = BestMemoryAdapter;
							}
						}
					}

					if (!FirstAdapter.IsValid())
					{
						FirstAdapter = CurrentAdapter;
					}
				}
			}
			else
			{
				E_LOG(Error, TEXT("  {}. Unknown, failed to create test device."), AdapterIndex);
			}
		}
		else
		{
			E_LOG(Log, TEXT("  {}. Unknown, failed to create adapter."), AdapterIndex);
		}
	}

	if (true /*bFavorDiscreteAdapter*/)
	{
		if (PreferredAdapter.IsValid())
		{
			ChosenAdapter = PreferredAdapter;
		}
		else if (BestMemoryAdapter.IsValid())
		{
			ChosenAdapter = BestMemoryAdapter;
		}
		else if (FirstDiscreteAdapter.IsValid())
		{
			ChosenAdapter = FirstDiscreteAdapter;
		}
		else
		{
			ChosenAdapter = FirstAdapter;
		}
	}
	else
	{
		ChosenAdapter = FirstAdapter;
	}

	if (ChosenAdapter.IsValid())
	{
		E_LOG(Log, TEXT("Chosen D3D11 Adapter:"));
		LogDXGIAdapterDesc(ChosenAdapter.DXGIAdapterDesc);
	}
	else
	{
		E_LOG(Error, TEXT("Failed to choose a D3D11 Adapter."));
	}

	/*GRHIAdapterName = ChosenAdapter.DXGIAdapterDesc.Description;
	GRHIVendorId = ChosenAdapter.DXGIAdapterDesc.VendorId;
	GRHIDeviceId = ChosenAdapter.DXGIAdapterDesc.DeviceId;
	GRHIDeviceRevision = ChosenAdapter.DXGIAdapterDesc.Revision;
	GRHIDeviceIsIntegrated = ChosenAdapter.bIsIntegrated;*/
}

void FD3D11DynamicRHI::InitD3DDevice()
{
	// UE는 더 이상 DEVICE_LOST 시의 정리 및 복구를 지원하지 않습니다.

	// 만약 아직 장치가 없거나, 첫 번째 뷰포트이거나, 기존 장치가 제거된 경우, 장치를 생성합니다.
	if (!Direct3DDevice)
	{
		E_LOG(Log, TEXT("Creating new Direct3DDevice"));

		ClearState();
		// Direct3D 11에서 하드웨어 또는 소프트웨어 장치를 생성하려는 경우, pAdapter를 NULL이 아닌 값으로 설정하여 다른 입력이 다음과 같이 제한되도록 합니다:
		//      DriverType는 D3D_DRIVER_TYPE_UNKNOWN이어야 합니다.
		//      Software는 NULL이어야 합니다.

		D3D_DRIVER_TYPE DriverType = D3D_DRIVER_TYPE_UNKNOWN;

		uint32 DeviceFlags = /*D3D11RHI_ShouldAllowAsyncResourceCreation()*/true ? 0 : D3D11_CREATE_DEVICE_SINGLETHREADED;

		if (GIsDebugLayerEnabled)
		{
			DeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
		}

		E_LOG(Log, TEXT("InitD3DDevice: -D3DDebug = {}"), GIsDebugLayerEnabled ? TEXT("on") : TEXT("off"));
		E_LOG(Log, TEXT("    GPU DeviceId: {:#x} (for the marketing name, search the web for \"GPU Device Id\")"), Adapter.DXGIAdapterDesc.DeviceId);
		
		D3D_FEATURE_LEVEL ActualFeatureLevel = (D3D_FEATURE_LEVEL)0;

		{
			E_LOG(Log, TEXT("Creating D3DDevice using adapter:"));
			LogDXGIAdapterDesc(Adapter.DXGIAdapterDesc);

			// Creating the Direct3D device.
			VERIFYD3D11RESULT(D3D11CreateDevice(
				Adapter.DXGIAdapter,
				DriverType,
				NULL,
				DeviceFlags,
				&FeatureLevel,
				1,
				D3D11_SDK_VERSION,
				Direct3DDevice.GetInitReference(),
				&ActualFeatureLevel,
				Direct3DDeviceIMContext.GetInitReference()
			));
		}

		// 요청한 기능 수준을 확보해야 합니다. 이를 지원하는지 확인
		_ASSERT(ActualFeatureLevel == FeatureLevel);

		if (GIsDebugLayerEnabled)
		{
			TRefCountPtr<ID3D11InfoQueue> d3dInfoQueue;
			if (SUCCEEDED(GD3D11RHI->GetDevice()->QueryInterface(__uuidof(ID3D11InfoQueue), (void**)d3dInfoQueue.GetInitReference())))
			{
				/* 콜백 설치 */
				ExceptionHandlerHandle = AddVectoredExceptionHandler(1, D3D11VectoredExceptionHandler);

				/* 메시지 필터링 */
				const bool bLogWarnings = true;// D3D11_ShouldBreakOnD3DDebugWarnings() || D3D11_ShouldLogD3DDebugWarnings();
				D3D11_INFO_QUEUE_FILTER NewFilter;
				ZeroMemory(&NewFilter, sizeof(NewFilter));

				D3D11_MESSAGE_SEVERITY DenySeverity[] = { D3D11_MESSAGE_SEVERITY_INFO, D3D11_MESSAGE_SEVERITY_WARNING };
				NewFilter.DenyList.NumSeverities = 1 + (bLogWarnings ? 0 : 1);
				NewFilter.DenyList.pSeverityList = DenySeverity;


				// 여기에 추가된 이유를 주의 깊게 설명해야 합니다! 나중에 누군가가 이를 보고 여전히 필요한지 파악할 수 있어야 합니다.
				D3D11_MESSAGE_ID DenyIds[] = {
					// OMSETRENDERTARGETS_INVALIDVIEW - 깊이 및 색상 타겟이 정확히 동일한 크기를 가지지 않으면 d3d가 불평합니다. 그러나 실제로
					//  색상 타겟이 더 작으면 문제가 없습니다. 그래서 이 오류를 끕니다. FD3D11DynamicRHI::SetRenderTarget에서 수동으로
					//  깊이가 색상보다 작고 MSAA 설정이 일치하는지 테스트합니다.
					D3D11_MESSAGE_ID_OMSETRENDERTARGETS_INVALIDVIEW,

					// QUERY_BEGIN_ABANDONING_PREVIOUS_RESULTS - RHI는 쿼리를 만들고 발행하는 인터페이스와 해당 데이터를 사용하는 별도의 인터페이스를 노출합니다.
					//      현재는 쿼리가 발행되고 결과가 의도적으로 무시될 수 있는 상황이 있습니다. 이 메시지를 필터링하여 디버그 메시지가 넘쳐 다른 중요한 경고를 가리는 것을 방지합니다.
					D3D11_MESSAGE_ID_QUERY_BEGIN_ABANDONING_PREVIOUS_RESULTS,
					D3D11_MESSAGE_ID_QUERY_END_ABANDONING_PREVIOUS_RESULTS,

					// D3D11_MESSAGE_ID_CREATEINPUTLAYOUT_EMPTY_LAYOUT - null 정점 선언을 사용할 때 트리거되는 경고입니다.
					//       이는 정점 셰이더가 ID를 기반으로 정점을 생성할 때 하려는 작업입니다.
					D3D11_MESSAGE_ID_CREATEINPUTLAYOUT_EMPTY_LAYOUT,

					// D3D11_MESSAGE_ID_DEVICE_DRAW_INDEX_BUFFER_TOO_SMALL - 이 경고는 Slate 드로우로 인해 트리거되며, 실제로는 유효한 인덱스 범위를 사용하고 있습니다.
					//      이 유효하지 않은 경고는 VS 2012가 설치된 경우에만 발생하는 것으로 보입니다. MS에 보고되었습니다.  
					//      지금은 DrawIndexedPrimitive에서 인덱스 버퍼의 범위를 벗어난 읽기 오류를 잡는 assert가 있습니다.
					D3D11_MESSAGE_ID_DEVICE_DRAW_INDEX_BUFFER_TOO_SMALL,

					// D3D11_MESSAGE_ID_DEVICE_DRAW_RENDERTARGETVIEW_NOT_SET - 이 경고는 섀도우 깊이 렌더링으로 인해 트리거되며, 셰이더가
					//      컬러를 출력하지만 컬러 렌더 타겟을 바인딩하지 않기 때문에 발생합니다. 이는 안전하며 바인딩되지 않은 렌더 타겟에 대한 쓰기가 폐기됩니다.
					//      또한, 씬 렌더링 외부에서 렌더링할 때 누적 요소로 인해 트리거되며, 이는 바인딩되지 않은 노말을 포함한 GBuffer로 출력합니다.
					(D3D11_MESSAGE_ID)3146081, // D3D11_MESSAGE_ID_DEVICE_DRAW_RENDERTARGETVIEW_NOT_SET,

					// 재사용되는 렌더타겟의 디버그 이름을 변경할 때 계속 스팸 메시지가 발생합니다.
					D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,

				};

				NewFilter.DenyList.NumIDs = sizeof(DenyIds) / sizeof(D3D11_MESSAGE_ID);
				NewFilter.DenyList.pIDList = (D3D11_MESSAGE_ID*)&DenyIds;

				d3dInfoQueue->PushStorageFilter(&NewFilter);

				/* ensure callback is called */
				/*d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, D3D11_ShouldBreakOnD3DDebugErrors());
				d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, D3D11_ShouldBreakOnD3DDebugErrors());
				if (bLogWarnings)
				{
					d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_WARNING, D3D11_ShouldBreakOnD3DDebugWarnings());
				}*/
				d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
				d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
				if (bLogWarnings)
				{
					d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_WARNING, true);
				}
			}
		}

		StateCache.Init(Direct3DDeviceIMContext);

		GRHICommandList.GetImmediateCommandList().InitializeImmediateContexts();

		FRenderResource::InitPreRHIResources();
	}
}


void FD3D11DynamicRHI::Init()
{
	InitD3DDevice();
}