#pragma once
#include "CoreMinimal.h"
#include "RenderResource.h"
#include "Shader.h"

struct FObjectUniformBuffer
{
	FMatrix WorldMatrix = FMatrix::Identity;
	FMatrix WorldInverseTransposeMatrix = FMatrix::Identity;
};

/**
 * 정점 셰이더에 연결될 수 있는 정점 데이터 소스를 캡슐화합니다.
 */
class RENDERCORE_API FVertexFactory : public FRenderResource
{
public:
	void Create(TObjectPtr<FVertexBuffer> InVertexBuffer, TObjectPtr<FIndexBuffer> InIndexBuffer, const FConstantBufferInfo& ConstantBufferInfo);
	void UpdateObjectUniformBuffer(FRHICommandList& CommandList, const FObjectUniformBuffer& InBuffer);

public:
	FVertexDeclarationRHIRef GetDeclaration() const { return Declaration; }
	FBufferRHIRef GetVertexBufferRHI() const { return VertexBuffer->VertexBufferRHI; }
	FBufferRHIRef GetIndexBufferRHI() const { return IndexBuffer->IndexBufferRHI; }

protected:
	/** 일반적으로 팩토리를 렌더링하는 데 사용되는 RHI 정점 선언. */
	FVertexDeclarationRHIRef Declaration;

	TObjectPtr<FVertexBuffer> VertexBuffer;
	TObjectPtr<FIndexBuffer> IndexBuffer;

	FObjectUniformBuffer ObjectUniformBuffer;

	// VS에서 사용할 WorldMatrix 등을 포함하는 UniformBuffer
	FUniformBufferRHIRef UniformBuffer;
};