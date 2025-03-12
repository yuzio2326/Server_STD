#include "Engine/StaticMesh.h"
#include "Factories/FbxFactory.h"

class ENGINE_API FStaticMeshVertexBuffer : public FVertexBuffer
{
public:
	TResourceArray<FPositionNormalUV> VertexData;

	virtual void InitRHI(FRHICommandList& RHICmdList) override
	{
		FRHIResourceCreateInfo CreateInfo(TEXT("VertexBuffer"), &VertexData);
		VertexBufferRHI = GetCommandList().CreateVertexBuffer(VertexData.GetResourceDataSize(), BUF_Static, CreateInfo);
		if (!VertexBufferRHI)
		{
			E_LOG(Warning, TEXT("VertexBufferRHI creation failed"));
			return;
		}
	}
};

class ENGINE_API FStaticMeshIndexBuffer : public FIndexBuffer
{
public:
	TResourceArray<uint32> IndexData;

	virtual void InitRHI(FRHICommandList& RHICmdList) override
	{
		FRHIResourceCreateInfo CreateInfo(TEXT("IndexBuffer"), &IndexData);
		IndexBufferRHI = GetCommandList().CreateIndexBuffer(IndexData.GetResourceDataSize(), IndexData.GetTypeSize(), BUF_Static, CreateInfo);
		if (!IndexBufferRHI)
		{
			E_LOG(Warning, TEXT("IndexBufferRHI creation failed"));
			return;
		}
	}
};

void FStaticMeshRenderData::Create(UStaticMesh* Outer, const FMeshData& NewMeshData)
{
	Name = NewMeshData.Name;

	Material = GetDefault<UMaterial>()->DefaultMaterial;

	TObjectPtr<FStaticMeshVertexBuffer> VertexBuffer = make_shared<FStaticMeshVertexBuffer>();
	VertexBuffer->VertexData = NewMeshData.Vertices;

	TObjectPtr<FStaticMeshIndexBuffer> IndexBuffer = make_shared<FStaticMeshIndexBuffer>();
	IndexBuffer->IndexData = NewMeshData.Indices;

	TShaderMapRef<FMaterialVS> VertexShader;
	VertexFactory.Create(VertexBuffer, IndexBuffer, VertexShader->GetConstantBufferInfo(TEXT("FObjectUniformBuffer")));

	NumVertices = NewMeshData.Vertices.size();
	NumPrimitives = NewMeshData.NumPrimitives;
}

UStaticMesh::UStaticMesh()
{

}

void UStaticMesh::Create(const TArray<FMeshData>& NewMeshData)
{
#if !SERVER
	RenderData.resize(NewMeshData.size());
	for (uint32 i = 0; i < RenderData.size(); ++i)
	{
		RenderData[i].Create(this, NewMeshData[i]);
	}
#endif
}