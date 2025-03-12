#pragma once
#include "CoreMinimal.h"
#include "VertexFactory.h"
#include "Materials/Material.h"
#include "StaticMesh.generated.h"

struct FMeshData;
class UMaterial;

struct FStaticMeshRenderData
{
public:
	void Create(class UStaticMesh* Outer, const FMeshData& NewMeshData);
	
	FString Name;
	FVertexFactory VertexFactory;

	TEnginePtr<UMaterial> Material;
	uint32 NumVertices = 0;
	uint32 NumPrimitives = 0;
};

UCLASS()
class ENGINE_API UStaticMesh : public UObject
{
	GENERATED_BODY()
public:
	UStaticMesh();

	virtual void Create(const TArray<FMeshData>& NewMeshData);

	TArray<FStaticMeshRenderData>& GetRenderData() { return RenderData; }

	TEnginePtr<UMaterial> GetMaterial(const int32 MaterialIndex)
	{
		return RenderData[MaterialIndex].Material;
	}

	uint32 GetRenderDataCount() { return RenderData.size(); }

protected:
	TArray<FStaticMeshRenderData> RenderData;
};