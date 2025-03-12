#pragma once
/*=============================================================================
	D3D11RHIPrivate.h: Private D3D RHI 정의.
=============================================================================*/

#include "DynamicRHI.h"
#include "D3D11Util.h"
#include "D3D11ThirdParty.h"
#include "D3D11StateCache.h"
#include "D3D11Resources.h"
#include "D3D11Viewport.h"
//#include "D3D11State.h"
#include "RHIContext.h"
#include "RHIShaderParameters.h"

// DX11 doesn't support higher MSAA count
#define DX_MAX_MSAA_COUNT 8

typedef ID3D11DeviceContext FD3D11DeviceContext;
typedef ID3D11Device FD3D11Device;

// 쉽게 생각해서 그래픽 카드에 대응되는 Struct라고 보면 됩니다
struct FD3D11Adapter
{
	/** 지원되지 않거나 FindAdapter()가 호출되지 않은 경우 null입니다. */
	TRefCountPtr<IDXGIAdapter> DXGIAdapter;

	DXGI_ADAPTER_DESC DXGIAdapterDesc = {};

	/** 지원되는 최대 D3D11 기능 수준입니다. 지원되지 않거나 FindAdapter()가 호출되지 않은 경우 0입니다. */
	D3D_FEATURE_LEVEL MaxSupportedFeatureLevel = {};

	/** 이것이 소프트웨어 어댑터인지 여부 */
	bool bSoftwareAdapter = false;

	/** GPU가 통합형인지 또는 개별형인지 여부를 나타냅니다. */
	bool bIsIntegrated = false;

	FD3D11Adapter()
	{
	}

	FD3D11Adapter(TRefCountPtr<IDXGIAdapter> InDXGIAdapter, D3D_FEATURE_LEVEL InMaxSupportedFeatureLevel, bool bInSoftwareAdatper, bool InIsIntegrated)
		: DXGIAdapter(InDXGIAdapter)
		, MaxSupportedFeatureLevel(InMaxSupportedFeatureLevel)
		, bSoftwareAdapter(bInSoftwareAdatper)
		, bIsIntegrated(InIsIntegrated)
	{
		if (DXGIAdapter.IsValid())
		{
			VERIFYD3D11RESULT(DXGIAdapter->GetDesc(&DXGIAdapterDesc));
		}
	}

	bool IsValid() const
	{
		return DXGIAdapter.IsValid();
	}
};

/** D3D11RHI 모듈을 동적 RHI 제공 모듈로 구현합니다. */
class FD3D11DynamicRHIModule : public IDynamicRHIModule
{
public:
	virtual void StartupModule() override;

	// IDynamicRHIModule
	virtual bool IsSupported() override;
	virtual FDynamicRHI* CreateRHI(ERHIFeatureLevel::Type RequestedFeatureLevel = ERHIFeatureLevel::Num) override;

private:
	FD3D11Adapter ChosenAdapter;

	// MaxSupportedFeatureLevel과 ChosenAdapter 설정
	void FindAdapter();
};

/** 동적으로 바인딩된 RHI가 구현하는 인터페이스입니다. */
class D3D11RHI_API FD3D11DynamicRHI : public FDynamicRHI, public IRHICommandContext
{
public:
	// Accessors.
	ID3D11Device* GetDevice() const
	{
		return Direct3DDevice;
	}
	FD3D11DeviceContext* GetDeviceContext() const
	{
		return Direct3DDeviceIMContext;
	}
	
	IDXGIFactory1* GetFactory() const
	{
		return DXGIFactory1;
	}

	// @return 0xffffffff if not not supported
	uint32 GetMaxMSAAQuality(uint32 SampleCount);

	template<typename TRHIType>
	static FORCEINLINE typename TD3D11ResourceTraits<TRHIType>::TConcreteType* ResourceCast(TRHIType* Resource)
	{
		return static_cast<typename TD3D11ResourceTraits<TRHIType>::TConcreteType*>(Resource);
	}

	static inline FD3D11Texture* ResourceCast(FRHITexture* Texture)
	{
		if (!Texture)
		{
			return nullptr;
		}

		FD3D11Texture* Result = static_cast<FD3D11Texture*>(Texture->GetTextureBaseRHI());
		_ASSERT(Result);

		return Result;
	}

	template<EShaderFrequency ShaderFrequency>
	void SetShaderParametersCommon(/*FD3D11ConstantBuffer* StageConstantBuffer, TConstArrayView<uint8> InParametersData, TConstArrayView<FRHIShaderParameter> InParameters,*/
		const TArray<FRHIShaderParameterResource>& InResourceParameters);

	template <EShaderFrequency ShaderFrequency>
	void SetShaderResourceView(FD3D11ViewableResource* Resource, ID3D11ShaderResourceView* SRV, int32 ResourceIndex)
	{
		InternalSetShaderResourceView<ShaderFrequency>(Resource, SRV, ResourceIndex);
	}

private:
	template <EShaderFrequency ShaderFrequency>
	void InternalSetShaderResourceView(FD3D11ViewableResource* Resource, ID3D11ShaderResourceView* SRV, int32 ResourceIndex);

public:
	virtual bool RHICompileShader(class FShaderType* InShaderType, TObjectPtr<class FShader>& OutShader);

public:
	virtual FViewportRHIRef RHICreateViewport(void* WindowHandle, uint32 SizeX, uint32 SizeY, bool bIsFullscreen, EPixelFormat PreferredPixelFormat) override;
	virtual class IRHICommandContext* RHIGetDefaultContext() final override;
	
	virtual FVertexShaderRHIRef RHICreateVertexShader(const TArray<uint8> Code, const type_index& Key/*Hash*/);
	virtual FPixelShaderRHIRef RHICreatePixelShader(const TArray<uint8> Code, const type_index& Key/*Hash*/);
	
	virtual FVertexDeclarationRHIRef RHICreateVertexDeclaration(const FVertexDeclarationElementList& Elements);
	virtual FBoundShaderStateRHIRef RHICreateBoundShaderState(FRHIVertexDeclaration* VertexDeclaration, FRHIVertexShader* VertexShader, FRHIPixelShader* PixelShader/*, FRHIGeometryShader* GeometryShader*/) final override;
	virtual void RHISetBoundShaderState(FRHIBoundShaderState* BoundShaderState) final override;
	
	virtual FBufferRHIRef RHICreateBuffer(FRHICommandList& RHICmdList, FRHIBufferDesc const& Desc, ERHIAccess ResourceState, FRHIResourceCreateInfo& CreateInfo) final override;
	virtual FUniformBufferRHIRef RHICreateUniformBuffer(const FConstantBufferInfo& Layout, const void* Contents, const uint32 ContentsSize);
	virtual void RHIUpdateUniformBuffer(FRHIUniformBuffer* UniformBufferRHI, const void* Contents, const uint32 ContentsSize) final override;
	virtual void RHISetShaderUniformBuffer(EShaderFrequency Frequency, uint8 RegisterIndex, FRHIUniformBuffer* InUniformBuffer) final override;

	virtual FTextureRHIRef RHICreateTexture(const FRHITextureCreateDesc& CreateDesc) final override;
	virtual FTextureRHIRef RHICreateTexture(const FString& InFilePath, const FString& InExtension) final override;
	virtual FTextureRHIRef RHICreateCubeTexture(const FString& InFilePath, const FString& InExtension) final override;

	virtual void RHISetViewport(float MinX, float MinY, float MinZ, float MaxX, float MaxY, float MaxZ) override;
	virtual void RHISetScissorRect(bool bEnable, uint32 MinX, uint32 MinY, uint32 MaxX, uint32 MaxY) final override;
	virtual void RHISetStreamSource(uint32 StreamIndex, FRHIBuffer* VertexBuffer, uint32 Offset) final override;
	virtual void RHISetPrimitiveTopology(EPrimitiveType InPrimitiveType) final override;
	virtual void RHIBeginDrawingViewport(FRHIViewport* Viewport, FRHITexture* RenderTargetRHI) final override;
	virtual void RHIEndDrawingViewport(FRHIViewport* Viewport, bool bPresent, bool bLockToVsync) final override;
	virtual void RHIClearMRTImpl(const bool* bClearColorArray, int32 NumClearColors, const FLinearColor* ColorArray, bool bClearDepth, float Depth, bool bClearStencil, uint32 Stencil);

	virtual void RHIDrawPrimitive(uint32 BaseVertexIndex, uint32 NumPrimitives, uint32 NumInstances) final override;
	virtual void RHIDrawIndexedPrimitive(FRHIBuffer* IndexBufferRHI, int32 BaseVertexIndex, uint32 FirstInstance, uint32 NumVertices, uint32 StartIndex, uint32 NumPrimitives, uint32 NumInstances) final override;

	virtual FSamplerStateRHIRef RHICreateSamplerState(const FSamplerStateInitializerRHI& Initializer) final override;
	virtual FRasterizerStateRHIRef RHICreateRasterizerState(const FRasterizerStateInitializerRHI& Initializer) final override;
	virtual FDepthStencilStateRHIRef RHICreateDepthStencilState(const FDepthStencilStateInitializerRHI& Initializer) final override;
	virtual void RHISetRasterizerState(FRHIRasterizerState* NewState) final override;
	virtual void RHISetDepthStencilState(FRHIDepthStencilState* NewState, uint32 StencilRef) final override;

	virtual void RHIBeginRenderPass(const FRHIRenderPassInfo& InInfo, const TCHAR* InName) final override;
	virtual void RHIEndRenderPass() final override;

	virtual void RHICopyTexture(FRHITexture* SourceTextureRHI, FRHITexture* DestTextureRHI, const FRHICopyTextureInfo& CopyInfo) final override;

	virtual void RHISetShaderParameters(FRHIGraphicsShader* Shader, TArray<FRHIShaderParameterResource>& InResourceParameters) final override;

public:
	void SetRenderTargets(uint32 NumSimultaneousRenderTargets, const FRHIRenderTargetView* NewRenderTargets, const FRHIDepthRenderTargetView* NewDepthStencilTarget);
	void SetRenderTargetsAndClear(const FRHISetRenderTargetsInfo& RenderTargetsInfo);
	void ConditionalClearShaderResource(FD3D11ViewableResource* Resource, bool bCheckBoundInputAssembler);

	void CommitRenderTargets(bool bClearUAVS);

public:
	/** Initialization constructor. */
	FD3D11DynamicRHI(IDXGIFactory1* InDXGIFactory1, D3D_FEATURE_LEVEL InFeatureLevel, const FD3D11Adapter& InAdapter);

	/** 아직 초기화되지 않은 경우, D3D 장치를 초기화합니다. */
	virtual void InitD3DDevice();

	/**
	 * Cleanup the D3D device.
	 * This function must be called from the main game thread.
	 */
	virtual void CleanupD3DDevice();

	// FDynamicRHI interface.
	virtual void Init() override;
	virtual void Shutdown() override;

public:
	void ClearState();
	//void ConditionalClearShaderResource(FD3D11ViewableResource* Resource, bool bCheckBoundInputAssembler);
	void ClearAllShaderResources();

	template <EShaderFrequency ShaderFrequency>
	void ClearAllShaderResourcesForFrequency();

protected:
	FD3D11Texture* CreateD3D11Texture2D(const FRHITextureCreateDesc& CreateDesc, TArray<D3D11_SUBRESOURCE_DATA> InitialData = {});
	void ValidateExclusiveDepthStencilAccess(FExclusiveDepthStencil Src) const;

private:
	void TrackResourceBoundAsVB(FD3D11ViewableResource* Resource, int32 StreamIndex);
	void TrackResourceBoundAsIB(FD3D11ViewableResource* Resource);

protected:
	/** The global D3D interface. */
	TRefCountPtr<IDXGIFactory1> DXGIFactory1;

	/** The feature level of the device. */
	D3D_FEATURE_LEVEL FeatureLevel;

	/** The global D3D device's immediate context */
	TRefCountPtr<FD3D11Device> Direct3DDevice;
	
	/** The global D3D device's immediate context */
	TRefCountPtr<FD3D11DeviceContext> Direct3DDeviceIMContext;

	HANDLE ExceptionHandlerHandle = INVALID_HANDLE_VALUE;

	/** 현재 그려지고 있는 뷰포트. */
	TRefCountPtr<FD3D11Viewport> DrawingViewport;

	TRefCountPtr<ID3D11DepthStencilView> CurrentDepthStencilTarget;
	TRefCountPtr<FD3D11Texture> CurrentDepthTexture;
	FD3D11ViewableResource* CurrentResourcesBoundAsSRVs[SF_NumStandardFrequencies][D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {};
	FD3D11ViewableResource* CurrentResourcesBoundAsVBs[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = {};
	FD3D11ViewableResource* CurrentResourceBoundAsIB = nullptr;
	int32 MaxBoundShaderResourcesIndex[SF_NumStandardFrequencies] = {};
	int32 MaxBoundVertexBufferIndex = -1;
	uint32 NumSimultaneousRenderTargets = 0;
	uint32 CurrentRTVOverlapMask = 0;
	uint32 CurrentUAVMask = 0;

	/** 현재 깊이 스텐실 접근 유형을 추적합니다. */
	FExclusiveDepthStencil CurrentDSVAccessType;

	// 현재 PSO 원시 유형
	EPrimitiveType PrimitiveType;

	TRefCountPtr<ID3D11RenderTargetView> CurrentRenderTargets[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];

protected:
	FD3D11Adapter Adapter;

	FD3D11StateCache StateCache;

protected:
	FRHIRenderPassInfo RenderPassInfo;
};