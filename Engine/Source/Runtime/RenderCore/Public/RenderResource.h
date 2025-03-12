#pragma once
#include "CoreMinimal.h"
#include "RHIFwd.h"
#include "RHIDefinitions.h"
#include "RHIResources.h"
#include "RHI.h"
#include "RHICommandList.h"

/**
 * 렌더링 스레드가 소유하는 렌더링 리소스입니다.
 */
class FRenderResource
{
public:
	////////////////////////////////////////////////////////////////////////////////////

	/**
	 * 렌더링 리소스의 초기화 순서를 제어합니다. 
	 * 초기 엔진 리소스는 정적 초기화 순서 문제를 피하기 위해 'Pre' 단계를 사용합니다.
	 */
	enum class EInitPhase : uint8
	{
		Pre,
		Default,
		MAX
	};

	/** 현재 초기화된 모든 렌더링 리소스를 해제합니다. */
	static RENDERCORE_API void ReleaseRHIForAllResources();

	/** RHI가 초기화되기 전에 초기화된 모든 리소스를 초기화합니다. */
	static RENDERCORE_API void InitPreRHIResources();

public:
	/** 기본 생성자. */
	RENDERCORE_API FRenderResource() {}

	/** 해제되지 않은 리소스를 감지하는 데 사용되는 소멸자. */ // 우리는 안하고 있음
	RENDERCORE_API virtual ~FRenderResource() {}

	/**
	 * 이 리소스에서 사용하는 RHI 리소스를 초기화합니다.
	 * 리소스와 RHI가 모두 초기화된 상태에 진입할 때 호출됩니다.
	 * 이는 렌더링 스레드에서만 호출됩니다.
	 */
	virtual void InitRHI(FRHICommandList& RHICmdList) {}

	/**
	 * 이 리소스에서 사용하는 RHI 리소스를 해제합니다.
	 * 리소스와 RHI가 모두 초기화된 상태를 벗어날 때 호출됩니다.
	 * 이는 렌더링 스레드에서만 호출됩니다.
	 */
	virtual void ReleaseRHI() {}

	/**
	 * 리소스를 초기화합니다.
	 * 이는 렌더링 스레드에서만 호출됩니다.
	 */
	RENDERCORE_API virtual void InitResource(/*FRHICommandList& RHICmdList*/);

	/**
	 * 리소스를 삭제할 준비를 합니다.
	 * 이는 렌더링 스레드에서만 호출됩니다.
	 */
	RENDERCORE_API virtual void ReleaseResource();
};

RENDERCORE_API TArray<FRenderResource*>& GetRenderResources();

/**
 * 정적 초기화/소멸에 의해 초기화/해제되는 렌더 리소스를 선언하는 데 사용됩니다.
 */
template<class ResourceType, FRenderResource::EInitPhase InInitPhase = FRenderResource::EInitPhase::Default>
class TGlobalResource : public ResourceType
{
public:
	/** 기본 생성자. */
	TGlobalResource()
	{
		InitGlobalResource();
	}

	/** Destructor. */
	virtual ~TGlobalResource()
	{
		ReleaseGlobalResource();
	}

	/**
	 * 글로벌 리소스를 초기화합니다.
	 */
	void InitGlobalResource()
	{
		// ResourceType::SetInitPhase(InInitPhase);

		((ResourceType*)this)->InitResource(/*FRenderResource::GetImmediateCommandList()*/);
	}


	/**
	 * 글로벌 리소스를 해제합니다.
	 */
	void ReleaseGlobalResource()
	{
//		// 이는 렌더링 스레드에서 호출되거나, 렌더링 스레드가 종료된 상태에서 종료 시 호출되어야 합니다.
//		// 그러나 오류 후 종료 시, 렌더링 스레드가 여전히 실행 중일 때 호출될 수도 있습니다.
//		// 그런 경우 두 번째 오류를 방지하기 위해 assert를 사용하지 않습니다.
//#if 0
//		check(IsInRenderingThread());
//#endif

		// 리소스를 정리합니다.
		((ResourceType*)this)->ReleaseResource();
	}

};

class FVertexDeclaration : public FRenderResource
{
public:
	~FVertexDeclaration() { ReleaseRHI(); }

	FVertexDeclarationRHIRef VertexDeclarationRHI;

	virtual void InitRHI(FRHICommandList& RHICmdList) override = 0;

	virtual void ReleaseRHI() override
	{
		VertexDeclarationRHI.SafeRelease();
	}
};

class FVertexBuffer : public FRenderResource
{
public:
	~FVertexBuffer() { ReleaseRHI(); }
	RENDERCORE_API virtual void InitRHI(FRHICommandList& RHICmdList) override = 0;
	RENDERCORE_API virtual void ReleaseRHI() override;

	FBufferRHIRef VertexBufferRHI;
};

class FIndexBuffer : public FRenderResource
{
public:
	~FIndexBuffer() { ReleaseRHI(); }
	RENDERCORE_API virtual void InitRHI(FRHICommandList& RHICmdList) override = 0;
	RENDERCORE_API virtual void ReleaseRHI() override;

	FBufferRHIRef IndexBufferRHI;
};

/**
 * Sends a message to the rendering thread to initialize a resource.
 * This is called in the game thread.
 */
extern RENDERCORE_API void BeginInitResource(FRenderResource* Resource);
