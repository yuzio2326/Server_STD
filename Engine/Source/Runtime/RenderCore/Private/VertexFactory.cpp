#include "VertexFactory.h"
#include "RenderCore.h"

class FStaticMeshVertexDeclaration : public FVertexDeclaration
{
public:
	virtual void InitRHI(FRHICommandList& RHICmdList) override
	{
		FVertexDeclarationElementList Elements;
		const uint16 Stride = sizeof(FPositionNormalUV);
		Elements.push_back(FVertexElement(0, STRUCT_OFFSET(FPositionNormalUV, Position), VET_Float3, 0, Stride));
		Elements.push_back(FVertexElement(0, STRUCT_OFFSET(FPositionNormalUV, Normal), VET_Float3, 1, Stride));
		Elements.push_back(FVertexElement(0, STRUCT_OFFSET(FPositionNormalUV, UV), VET_Float2, 2, Stride));
		VertexDeclarationRHI = GDynamicRHI->RHICreateVertexDeclaration(Elements);
	}
};
TGlobalResource<FStaticMeshVertexDeclaration> GStaticMeshVertexDeclaration;

void FVertexFactory::Create(TObjectPtr<FVertexBuffer> InVertexBuffer, TObjectPtr<FIndexBuffer> InIndexBuffer, const FConstantBufferInfo& ConstantBufferInfo)
{
	Declaration = GStaticMeshVertexDeclaration.VertexDeclarationRHI;
	VertexBuffer = InVertexBuffer;
	VertexBuffer->InitRHI(FRHICommandListExecutor::GetImmediateCommandList());

	IndexBuffer = InIndexBuffer;
	IndexBuffer->InitRHI(FRHICommandListExecutor::GetImmediateCommandList());

	UniformBuffer = RHICreateUniformBuffer(ConstantBufferInfo, &ObjectUniformBuffer, sizeof(ObjectUniformBuffer));
}

void FVertexFactory::UpdateObjectUniformBuffer(FRHICommandList& CommandList, const FObjectUniformBuffer& InBuffer)
{
	ObjectUniformBuffer = InBuffer;
	ObjectUniformBuffer.WorldMatrix = ObjectUniformBuffer.WorldMatrix.Transpose();
	ObjectUniformBuffer.WorldInverseTransposeMatrix = ObjectUniformBuffer.WorldMatrix.Invert().Transpose();
	RHIUpdateUniformBuffer(UniformBuffer, &ObjectUniformBuffer, sizeof(ObjectUniformBuffer));
	CommandList.SetShaderUniformBuffer(EShaderFrequency::SF_Vertex, UniformBuffer);
}
