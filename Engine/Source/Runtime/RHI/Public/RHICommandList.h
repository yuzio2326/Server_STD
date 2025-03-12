#pragma once
#include "CoreMinimal.h"
#include "DynamicRHI.h"
#include "RHIContext.h"
#include "RHIAccess.h"
#include "RHIResources.h"
#include "RHIShaderParameters.h"

/** 주어진 텍스처 생성 플래그에 대한 최적의 기본 리소스 상태를 가져옵니다 */
extern RHI_API ERHIAccess RHIGetDefaultResourceState(ETextureCreateFlags InUsage, bool bInHasInitialData);

/** 주어진 버퍼 생성 플래그에 대한 최적의 기본 리소스 상태를 가져옵니다 */
extern RHI_API ERHIAccess RHIGetDefaultResourceState(EBufferUsageFlags InUsage, bool bInHasInitialData);

class FRHICommandList
{
public:
	RHI_API void InitializeImmediateContexts();
	
	FORCEINLINE IRHICommandContext& GetContext()
	{
		_ASSERT(GraphicsContext, TEXT("There is no active graphics context on this command list. There may be a missing call to SwitchPipeline()."));
		return *GraphicsContext;
	}

	RHI_API void BeginDrawingViewport(FRHIViewport* Viewport, FRHITexture* RenderTargetRHI);
	RHI_API void EndDrawingViewport(FRHIViewport* Viewport, bool bPresent, bool bLockToVsync);
	
	/*FORCEINLINE void* LockBuffer(FRHIBuffer* Buffer, uint32 Offset, uint32 SizeRHI, EResourceLockMode LockMode)
	{
		FRHICommandListScopedPipelineGuard ScopedPipeline(*this);
		return GDynamicRHI->RHILockBuffer(*this, Buffer, Offset, SizeRHI, LockMode);
	}

	FORCEINLINE void UnlockBuffer(FRHIBuffer* Buffer)
	{
		FRHICommandListScopedPipelineGuard ScopedPipeline(*this);
		GDynamicRHI->RHIUnlockBuffer(*this, Buffer);
	}*/

	FORCEINLINE FBufferRHIRef CreateBuffer(uint32 Size, EBufferUsageFlags Usage, uint32 Stride, ERHIAccess ResourceState, FRHIResourceCreateInfo& CreateInfo)
	{
		FRHIBufferDesc BufferDesc = CreateInfo.bWithoutNativeResource
			? FRHIBufferDesc::Null()
			: FRHIBufferDesc(Size, Stride, Usage);

		//FRHICommandListScopedPipelineGuard ScopedPipeline(*this);
		FBufferRHIRef Buffer = GDynamicRHI->RHICreateBuffer(*this, BufferDesc, ResourceState, CreateInfo);
		Buffer->SetTrackedAccess_Unsafe(ResourceState);
		return Buffer;
	}

	FORCEINLINE FBufferRHIRef CreateVertexBuffer(uint32 Size, EBufferUsageFlags Usage, ERHIAccess ResourceState, FRHIResourceCreateInfo& CreateInfo)
	{
		return CreateBuffer(Size, Usage | EBufferUsageFlags::VertexBuffer, 0, ResourceState, CreateInfo);
	}
	FORCEINLINE FBufferRHIRef CreateVertexBuffer(uint32 Size, EBufferUsageFlags Usage, FRHIResourceCreateInfo& CreateInfo)
	{
		bool bHasInitialData = CreateInfo.BulkData != nullptr;
		ERHIAccess ResourceState = RHIGetDefaultResourceState(Usage | EBufferUsageFlags::VertexBuffer, bHasInitialData);
		return CreateVertexBuffer(Size, Usage, ResourceState, CreateInfo);
	}

	FORCEINLINE FBufferRHIRef CreateIndexBuffer(uint32 Size, uint32 Stride, EBufferUsageFlags Usage, ERHIAccess ResourceState, FRHIResourceCreateInfo& CreateInfo)
	{
		return CreateBuffer(Size, Usage | EBufferUsageFlags::IndexBuffer, Stride, ResourceState, CreateInfo);
	}

	FORCEINLINE FBufferRHIRef CreateIndexBuffer(uint32 Size, uint32 Stride, EBufferUsageFlags Usage, FRHIResourceCreateInfo& CreateInfo)
	{
		bool bHasInitialData = CreateInfo.BulkData != nullptr;
		ERHIAccess ResourceState = RHIGetDefaultResourceState(Usage | EBufferUsageFlags::IndexBuffer, bHasInitialData);
		return CreateIndexBuffer(Size, Stride, Usage, ResourceState, CreateInfo);
	}

	RHI_API void SetBoundShaderState(FRHIBoundShaderState* BoundShaderState);

	FORCEINLINE void SetShaderUniformBuffer(EShaderFrequency Frequency, FRHIUniformBuffer* InUniformBuffer)
	{
		GetContext().RHISetShaderUniformBuffer(Frequency, InUniformBuffer->GetLayout().RegisterIndex, InUniformBuffer);
	}

	FORCEINLINE void SetStreamSource(uint32 StreamIndex, FRHIBuffer* VertexBuffer, uint32 Offset)
	{
		GetContext().RHISetStreamSource(StreamIndex, VertexBuffer, Offset);
	}

	FORCEINLINE void SetPrimitiveTopology(EPrimitiveType InPrimitiveType)
	{
		GetContext().RHISetPrimitiveTopology(InPrimitiveType);
	}

	FORCEINLINE void DrawPrimitive(uint32 BaseVertexIndex, uint32 NumPrimitives, uint32 NumInstances)
	{
		GetContext().RHIDrawPrimitive(BaseVertexIndex, NumPrimitives, NumInstances);
	}

	FORCEINLINE void DrawIndexedPrimitive(FRHIBuffer* IndexBufferRHI, int32 BaseVertexIndex, uint32 FirstInstance, uint32 NumVertices, uint32 StartIndex, uint32 NumPrimitives, uint32 NumInstances)
	{
		GetContext().RHIDrawIndexedPrimitive(IndexBufferRHI, BaseVertexIndex, FirstInstance, NumVertices, StartIndex, NumPrimitives, NumInstances);
	}

	FORCEINLINE void SetRasterizerState(FRHIRasterizerState* RasterizerState)
	{
		GetContext().RHISetRasterizerState(RasterizerState);
	}

	FORCEINLINE void SetDepthStencilState(FRHIDepthStencilState* DepthStencilState, uint32 StencilRef = 0)
	{
		GetContext().RHISetDepthStencilState(DepthStencilState, StencilRef);
	}

	FORCEINLINE FTextureRHIRef CreateTexture(const FRHITextureCreateDesc& CreateDesc)
	{
		if (CreateDesc.InitialState == ERHIAccess::Unknown)
		{
			FRHITextureCreateDesc NewCreateDesc(CreateDesc);
			NewCreateDesc.SetInitialState(RHIGetDefaultResourceState(CreateDesc.Flags, CreateDesc.BulkData != nullptr));

			return GDynamicRHI->RHICreateTexture(NewCreateDesc);
		}
		return GDynamicRHI->RHICreateTexture(CreateDesc);
	}

	FORCEINLINE FTextureRHIRef CreateTexture(const FString& InFilePath, const FString& InExtension)
	{
		return GDynamicRHI->RHICreateTexture(InFilePath, InExtension);
	}

	FORCEINLINE FTextureRHIRef GetTexture(const FString& InFilePath)
	{
		return GDynamicRHI->RHICreateTexture(InFilePath, FString());
	}

	FORCEINLINE void BeginRenderPass(const FRHIRenderPassInfo& InInfo, const TCHAR* Name)
	{
		_ASSERT(!IsInsideRenderPass());
		//check(!IsInsideComputePass());

		//InInfo.Validate();

		GetContext().RHIBeginRenderPass(InInfo, Name);

		CacheActiveRenderTargets(InInfo);
		PersistentState.bInsideRenderPass = true;
		/*ResetSubpass(InInfo.SubpassHint);


		if (InInfo.NumOcclusionQueries)
		{
			PersistentState.bInsideOcclusionQueryBatch = true;
			GDynamicRHI->RHIBeginOcclusionQueryBatch_TopOfPipe(*this, InInfo.NumOcclusionQueries);
		}*/
	}

	FORCEINLINE void EndRenderPass()
	{
		_ASSERT(IsInsideRenderPass());

		GetContext().RHIEndRenderPass();

		PersistentState.bInsideRenderPass = false;
	}

	bool IsInsideRenderPass() const { return PersistentState.bInsideRenderPass; }

	RHI_API void CopyTexture(FRHITexture* SourceTextureRHI, FRHITexture* DestTextureRHI, const FRHICopyTextureInfo& CopyInfo);

	/*void BeginRenderPass(function<void()> InCmd)
	{
		RenderCmds.emplace_back(InCmd);
	}

	void ExecuteRenderPass()
	{
		for (function Cmd : RenderCmds)
		{
			Cmd();
		}
		RenderCmds.clear();
	}*/

	RHI_API void SetShaderParameters(
		FRHIGraphicsShader* InShader
		/*, TArray<uint8>& InParametersData
		, TArray<FRHIShaderParameter>& InParameters*/
		, TArray<FRHIShaderParameterResource>& InResourceParameters
		/*, TArray<FRHIShaderParameterResource> InBindlessParameters*/
	);

protected:
	void CacheActiveRenderTargets(const FRHIRenderPassInfo& Info)
	{
		FRHISetRenderTargetsInfo RTInfo;
		Info.ConvertToRenderTargetsInfo(RTInfo);

		for (int32 RTIdx = 0; RTIdx < RTInfo.NumColorRenderTargets; ++RTIdx)
		{
			PersistentState.CachedRenderTargets[RTIdx] = RTInfo.ColorRenderTarget[RTIdx];
		}

		PersistentState.CachedNumSimultanousRenderTargets = RTInfo.NumColorRenderTargets;
		PersistentState.CachedDepthStencilTarget = RTInfo.DepthStencilRenderTarget;
		//PersistentState.HasFragmentDensityAttachment = RTInfo.ShadingRateTexture != nullptr;
		//PersistentState.MultiViewCount = RTInfo.MultiViewCount;
	}

	// 이 구조체의 값들은 명령 목록이 이동되거나 재설정될 때 유지됩니다.
	struct FPersistentState
	{
		uint32 CachedNumSimultanousRenderTargets = 0; // 캐시된 동시 렌더 타겟 수
		TStaticArray<FRHIRenderTargetView, MaxSimultaneousRenderTargets> CachedRenderTargets; // 캐시된 렌더 타겟 배열
		FRHIDepthRenderTargetView CachedDepthStencilTarget; // 캐시된 깊이-스텐실 타겟

		//ESubpassHint SubpassHint = ESubpassHint::None; // 서브패스 힌트
		//uint8 SubpassIndex = 0; // 서브패스 인덱스
		//uint8 MultiViewCount = 0; // 멀티뷰 카운트
		//uint8 ExtendResourceLifetimeRefCount = 0; // 리소스 수명 연장 참조 카운트
		//bool HasFragmentDensityAttachment = false; // 프래그먼트 밀도 첨부 여부

		bool bInsideRenderPass = false; // 렌더 패스 내부 여부
		//bool bInsideComputePass = false; // 컴퓨트 패스 내부 여부
		//bool bInsideOcclusionQueryBatch = false; // 오클루전 쿼리 배치 내부 여부
		//bool bAsyncPSOCompileAllowed = true; // 비동기 PSO 컴파일 허용 여부
		//bool bImmediate = false; // 즉시 여부

		//ERecordingThread RecordingThread; // 기록 스레드

		//FRHIGPUMask CurrentGPUMask; // 현재 GPU 마스크
		//FRHIGPUMask InitialGPUMask; // 초기 GPU 마스크

		//FBoundShaderStateInput BoundShaderInput; // 바인딩된 셰이더 상태 입력
		//FRHIComputeShader* BoundComputeShaderRHI = nullptr; // 바인딩된 컴퓨트 셰이더 RHI

		//FGraphEventRef RHIThreadBufferLockFence; // RHI 스레드 버퍼 잠금 펜스

		//struct FFenceCandidate : public TConcurrentLinearObject<FFenceCandidate>, public FRefCountBase
		//{
		//	FGraphEventRef Fence; // 펜스
		//};

		//TRefCountPtr<FFenceCandidate> FenceCandidate; // 펜스 후보
		//FGraphEventArray QueuedFenceCandidateEvents; // 큐에 저장된 펜스 후보 이벤트 배열
		//TArray<TRefCountPtr<FFenceCandidate>, FConcurrentLinearArrayAllocator> QueuedFenceCandidates; // 큐에 저장된 펜스 후보 배열
		//TArray<FRHIResource*, FConcurrentLinearArrayAllocator> ExtendedLifetimeResources; // 수명이 연장된 리소스 배열

//		struct FGPUStats
//		{
//#if HAS_GPU_STATS
//			FDrawCallCategoryName* CategoryTOP = nullptr; // 상위 카테고리
//			FDrawCallCategoryName* CategoryBOP = nullptr; // 하위 카테고리
//#endif
//
//			FRHIDrawStats* Ptr = nullptr; // 드로우 통계 포인터
//
//			void InitFrom(FGPUStats* Other)
//			{
//				if (!Other)
//					return;
//
//				Ptr = Other->Ptr;
//#if HAS_GPU_STATS
//				CategoryBOP = Other->CategoryTOP;
//#endif
//			}
//
//			void ApplyToContext(IRHIComputeContext* Context)
//			{
//				uint32 CategoryID = FRHIDrawStats::NoCategory;
//#if HAS_GPU_STATS
//				check(!CategoryBOP || CategoryBOP->ShouldCountDraws());
//				if (CategoryBOP)
//				{
//					CategoryID = CategoryBOP->Index;
//				}
//#endif
//
//				Context->StatsSetCategory(Ptr, CategoryID);
//			}
//		} Stats; // GPU 통계

		/*FPersistentState(FRHIGPUMask InInitialGPUMask, ERecordingThread InRecordingThread, bool bInImmediate = false)
			: bImmediate(bInImmediate)
			, RecordingThread(InRecordingThread)
			, CurrentGPUMask(InInitialGPUMask)
			, InitialGPUMask(InInitialGPUMask)
		{
		}*/
	} PersistentState; // 지속 상태

private:
	// 그래픽 명령이 기록되는 활성 컨텍스트.
	IRHICommandContext* GraphicsContext = nullptr;
	//TArray<function<void()>> RenderCmds;
};

class FRHICommandListExecutor
{
public:
	FRHICommandListExecutor()
	{
	}

	// GetImmediateCommandList
	static inline FRHICommandList& GetImmediateCommandList();

private:
	FRHICommandList CommandListImmediate;
};
extern RHI_API FRHICommandListExecutor GRHICommandList;

inline FRHICommandList& FRHICommandListExecutor::GetImmediateCommandList()
{
	//check(IsInRenderingThread());
	return GRHICommandList.CommandListImmediate;
}

inline FRHICommandList& GetCommandList()
{
	return FRHICommandListExecutor::GetImmediateCommandList();
}