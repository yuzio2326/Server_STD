#pragma once
class FRHIViewport;
class FRHITexture;
class FRHIBoundShaderState;
class FRHIBuffer;
class FRHIUniformBuffer;
class FRHIRasterizerState;
class FRHIDepthStencilState;
class FRHIGraphicsShader;
struct FRHIRenderPassInfo;
struct FRHIShaderParameterResource;

struct FRHICopyTextureInfo
{
    // 소스 텍스처의 복사할 영역을 반환합니다.
    FVector4D GetSourceRect() const
    {
        return FVector4D(SourcePosition.x, SourcePosition.y, SourcePosition.x + Size.x, SourcePosition.y + Size.y);
    }

    // 목적지 텍스처의 복사할 영역을 반환합니다.
    FVector4D GetDestRect() const
    {
        return FVector4D(DestPosition.x, DestPosition.y, DestPosition.x + Size.x, DestPosition.y + Size.y);
    }

    // 복사할 텍셀의 수입니다. 기본적으로 크기가 지정되지 않으면 전체 리소스를 복사합니다.
    FVector3D Size = FVector3D::Zero;

    // 소스 텍스처에서 복사할 위치/목적지 텍스처로의 위치
    FVector3D SourcePosition = FVector3D::Zero;
    FVector3D DestPosition = FVector3D::Zero;

    uint32 SourceSliceIndex = 0;
    uint32 DestSliceIndex = 0;
    uint32 NumSlices = 1;

    // 복사할 Mip 레벨과 목적지 Mip 레벨
    uint32 SourceMipIndex = 0;
    uint32 DestMipIndex = 0;
    uint32 NumMips = 1;
};

// RHI 명령 컨텍스트 인터페이스. 때로는 RHI가 이를 처리합니다.
// 명령 리스트를 병렬로 처리할 수 있는 플랫폼에서는 별도의 객체입니다.
class IRHICommandContext
{
public:
	virtual ~IRHICommandContext()
	{
	}

public:
	virtual void RHISetBoundShaderState(FRHIBoundShaderState* BoundShaderState) = 0;
	virtual void RHISetStreamSource(uint32 StreamIndex, FRHIBuffer* VertexBuffer, uint32 Offset) = 0;
	virtual void RHISetPrimitiveTopology(EPrimitiveType InPrimitiveType) = 0;
	virtual void RHISetShaderUniformBuffer(EShaderFrequency Frequency, uint8 RegisterIndex, FRHIUniformBuffer* InUniformBuffer) = 0;
	virtual void RHISetScissorRect(bool bEnable, uint32 MinX, uint32 MinY, uint32 MaxX, uint32 MaxY) = 0;

	virtual void RHIDrawPrimitive(uint32 BaseVertexIndex, uint32 NumPrimitives, uint32 NumInstances) = 0;
	virtual void RHIDrawIndexedPrimitive(FRHIBuffer* IndexBufferRHI, int32 BaseVertexIndex, uint32 FirstInstance, uint32 NumVertices, uint32 StartIndex, uint32 NumPrimitives, uint32 NumInstances) = 0;

	// 이 메서드는 RHIThread와 함께 대기열에 추가되며, 그렇지 않으면 대기열에 추가된 후 플러시됩니다. 
	// RHI 스레드가 없으면 이 프레임 전진 명령을 대기열에 추가하는 데 이점이 없습니다.
	virtual void RHIBeginDrawingViewport(FRHIViewport* Viewport, FRHITexture* RenderTargetRHI) = 0;

	// 이 메서드는 RHIThread와 함께 대기열에 추가되며, 그렇지 않으면 대기열에 추가된 후 플러시됩니다.
	// RHI 스레드가 없으면 이 프레임 전진 명령을 대기열에 추가하는 데 이점이 없습니다.
	virtual void RHIEndDrawingViewport(FRHIViewport* Viewport, bool bPresent, bool bLockToVsync) = 0;

	virtual void RHIClearMRTImpl(const bool* bClearColorArray, int32 NumClearColors, const FLinearColor* ColorArray, bool bClearDepth, float Depth, bool bClearStencil, uint32 Stencil) = 0;

	virtual void RHISetViewport(float MinX, float MinY, float MinZ, float MaxX, float MaxY, float MaxZ) = 0;
	virtual void RHISetRasterizerState(FRHIRasterizerState* NewState) = 0;
    virtual void RHISetDepthStencilState(FRHIDepthStencilState* NewState, uint32 StencilRef) = 0;

	virtual void RHIBeginRenderPass(const FRHIRenderPassInfo& InInfo, const TCHAR* InName) = 0;
	virtual void RHIEndRenderPass() = 0;

    virtual void RHICopyTexture(FRHITexture* SourceTextureRHI, FRHITexture* DestTextureRHI, const FRHICopyTextureInfo& CopyInfo) = 0;

    virtual void RHISetShaderParameters(FRHIGraphicsShader* Shader, TArray<FRHIShaderParameterResource>& InResourceParameters) = 0;
};