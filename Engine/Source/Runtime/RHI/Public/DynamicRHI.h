#pragma once
#include "CoreMinimal.h"
#include "RHIFeatureLevel.h"
#include "RHIDefinitions.h"
#include "RHIUtilites.h"
#include "RHIContext.h"
//#include "RHICommandList.h"
#include "RHIFwd.h"
#include "RHIAccess.h"
#include "PixelFormat.h"
#include "../../Engine/Public/SceneUtils.h"

class FRHICommandList;
struct FRHIBufferDesc;
struct FRHIResourceCreateInfo;
struct FRHITextureCreateDesc;

/** 동적으로 바인딩된 RHI가 구현하는 인터페이스입니다. */
class FDynamicRHI
{
public:
	/** 동적 RHI를 타입을 알 필요 없이 삭제할 수 있도록 가상 소멸자를 선언합니다. */
	virtual ~FDynamicRHI() {}

	/** GDynamicRHI가 호출될 때 설정되도록 IDynamicRHIModule::CreateRHI와 분리된 RHI를 초기화합니다. */
	virtual void Init() = 0;
	/** RHI의 실제 소멸자가 호출되기 전에 종료 및 리소스 파괴를 처리하여 RHI의 모든 리소스가 종료를 위해 여전히 사용 가능하도록 합니다. */
	virtual void Shutdown() = 0;

	virtual IRHICommandContext* RHIGetDefaultContext() = 0;
	virtual FViewportRHIRef RHICreateViewport(void* WindowHandle, uint32 SizeX, uint32 SizeY, bool bIsFullscreen, EPixelFormat PreferredPixelFormat) = 0;
	
	virtual FVertexShaderRHIRef RHICreateVertexShader(const TArray<uint8> Code, const type_index& Key/*Hash*/) = 0;
	virtual FPixelShaderRHIRef RHICreatePixelShader(const TArray<uint8> Code, const type_index& Key/*Hash*/) = 0;
	
	virtual FVertexDeclarationRHIRef RHICreateVertexDeclaration(const FVertexDeclarationElementList& Elements) = 0;
	virtual FBoundShaderStateRHIRef RHICreateBoundShaderState(FRHIVertexDeclaration* VertexDeclaration, FRHIVertexShader* VertexShader, FRHIPixelShader* PixelShader/*, FRHIGeometryShader* GeometryShader*/) = 0;
	virtual FBufferRHIRef RHICreateBuffer(FRHICommandList& RHICmdList, FRHIBufferDesc const& Desc, ERHIAccess ResourceState, FRHIResourceCreateInfo& CreateInfo) = 0;
	virtual FUniformBufferRHIRef RHICreateUniformBuffer(const FConstantBufferInfo& Layout, const void* Contents, const uint32 ContentsSize) = 0;
	virtual void RHIUpdateUniformBuffer(FRHIUniformBuffer* UniformBufferRHI, const void* Contents, const uint32 ContentsSize) = 0;

	virtual bool RHICompileShader(class FShaderType* InShaderType, TObjectPtr<class FShader>& OutShader) = 0;
	virtual FTextureRHIRef RHICreateTexture(const FRHITextureCreateDesc& CreateDesc) = 0;
	virtual FTextureRHIRef RHICreateTexture(const FString& InFilePath, const FString& InExtension) = 0;
	virtual FTextureRHIRef RHICreateCubeTexture(const FString& InFilePath, const FString& InExtension) = 0;

	virtual FSamplerStateRHIRef RHICreateSamplerState(const FSamplerStateInitializerRHI& Initializer) = 0;
	virtual FRasterizerStateRHIRef RHICreateRasterizerState(const FRasterizerStateInitializerRHI& Initializer) = 0;
	virtual FDepthStencilStateRHIRef RHICreateDepthStencilState(const FDepthStencilStateInitializerRHI& Initializer) = 0;
};

/** 동적 RHI(랜더링 하드웨어 인터페이스)를 구현하는 모듈의 인터페이스를 정의합니다. */
class IDynamicRHIModule : public IModuleInterface
{
public:

	/** Checks whether the RHI is supported by the current system. */
	virtual bool IsSupported() = 0;

	virtual bool IsSupported(ERHIFeatureLevel::Type RequestedFeatureLevel) { return IsSupported(); }

	/** Creates a new instance of the dynamic RHI implemented by the module. */
	virtual FDynamicRHI* CreateRHI(ERHIFeatureLevel::Type RequestedFeatureLevel = ERHIFeatureLevel::Num) = 0;
};

/**
*   동적 RHI를 사용하는 각 플랫폼은 이 함수를 구현해야 합니다.
*   동적 RHI 인스턴스를 생성하기 위해 호출됩니다.
*/
FDynamicRHI* PlatformCreateDynamicRHI();


// 동적으로 바인딩된 RHI 구현을 가리키는 전역 포인터.
extern RHI_API FDynamicRHI* GDynamicRHI;

FORCEINLINE class IRHICommandContext* RHIGetDefaultContext()
{
	return GDynamicRHI->RHIGetDefaultContext();
}

FUniformBufferRHIRef RHI_API RHICreateUniformBuffer(const FConstantBufferInfo& Layout, const void* Contents, const uint32 ContentsSize);

FORCEINLINE void RHIUpdateUniformBuffer(FRHIUniformBuffer* UniformBufferRHI, const void* Contents, const uint32 ContentsSize)
{
	GDynamicRHI->RHIUpdateUniformBuffer(UniformBufferRHI, Contents, ContentsSize);
}

FSamplerStateRHIRef RHI_API RHICreateSamplerState(const FSamplerStateInitializerRHI& Initializer);
FRasterizerStateRHIRef RHI_API RHICreateRasterizerState(const FRasterizerStateInitializerRHI& Initializer);
FDepthStencilStateRHIRef RHI_API RHICreateDepthStencilState(const FDepthStencilStateInitializerRHI& Initializer);

EShadingPath RHI_API GetShadingPath();