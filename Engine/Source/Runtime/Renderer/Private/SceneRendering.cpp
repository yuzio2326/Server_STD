#include "RendererModule.h"
#include "ScenePrivate.h"
#include "SceneView.h"
#include "SceneRendering.h"
#include "StaticMeshSceneProxy.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "RHIStaticStates.h"

#include "Engine/Engine.h"
#include "GameFramework/Actor.h"
#include "Components/DirectionalLightComponent.h"
#include "LightSceneProxy.h"
#include "RenderCore.h"

FRendererModule::FRendererModule()
{
}

void FRendererModule::StartupModule()
{
	IRendererModule::StartupModule();
}

void FRendererModule::ShutdownModule()
{
	IRendererModule::ShutdownModule();
}

FSceneInterface* FRendererModule::AllocateScene(UWorld* World, ERHIFeatureLevel::Type InFeatureLevel)
{
	FScene* NewScene = new FScene(World, InFeatureLevel);
	AllocatedScenes.insert(NewScene);
	return NewScene;
}

void FRendererModule::RemoveScene(FSceneInterface* Scene)
{
	AllocatedScenes.erase(Scene);
	delete Scene;
}

void FRendererModule::BeginRenderingViewFamily(FSceneViewFamily* ViewFamily)
{
	// FSceneRenderer::CreateSceneRenderers(ViewFamiliesConst, Canvas->GetHitProxyConsumer(), SceneRenderers);
	// RenderViewFamilies_RenderThread

	FSceneRenderer* SceneRenderer = new FSceneRenderer(ViewFamily);

	{
		SceneRenderer->Render();
	}
	delete SceneRenderer;
}

FSceneRenderer::FSceneRenderer(const FSceneViewFamily* InViewFamily)
	: ViewFamily(*InViewFamily)
{
}

FSceneRenderer::~FSceneRenderer()
{
}

struct FStaticMeshDrawCommand
{
	FStaticMeshDrawCommand(FPrimitiveSceneProxy* InProxy, TArray<FStaticMeshRenderData>& InRenderDatas, TArray<TEnginePtr<UMaterial>>& InOverrideMaterials)
		: Proxy(InProxy), RenderDatas(InRenderDatas), OverrideMaterials(InOverrideMaterials)
	{
	}

	FPrimitiveSceneProxy* Proxy;
	TArray<FStaticMeshRenderData>& RenderDatas;
	TArray<TEnginePtr<UMaterial>>& OverrideMaterials;
};


#include "Shader.h"
class FTestVS : public FShader
{
	DECLARE_SHADER_TYPE(FTestVS)
};
IMPLEMENT_SHADER_TYPE(FTestVS, FPaths::ShaderDir() + L"/VertexShader.hlsl", "VS", SF_Vertex)

class FTestPS : public FShader
{
	DECLARE_SHADER_TYPE(FTestPS)
};
IMPLEMENT_SHADER_TYPE(FTestPS, FPaths::ShaderDir() + L"/PixelShader.hlsl", "PS", SF_Pixel)

class FPositionVertexDeclaration : public FVertexDeclaration
{
public:
	virtual void InitRHI(FRHICommandList& RHICmdList) override
	{
		FVertexDeclarationElementList Elements;
		Elements.push_back(FVertexElement(0, 0, VET_Float3, 0, sizeof(FVector3D)));
		VertexDeclarationRHI = GDynamicRHI->RHICreateVertexDeclaration(Elements);
	}
};
TGlobalResource<FPositionVertexDeclaration> GPositionVertexDeclaration;

class FNDCTriangleVertexBuffer : public FVertexBuffer
{
public:
	virtual void InitRHI(FRHICommandList& RHICmdList)
	{
		TResourceArray<FVector3D> PositionData;
		PositionData.emplace_back(-0.5f, -0.5f, 0.0f);
		PositionData.emplace_back(0.5f, -0.5f, 0.0f);
		PositionData.emplace_back(0.0f, 0.5f, 0.0f);

		FRHIResourceCreateInfo CreateInfo(TEXT("TriangleVertexBuffer"), &PositionData);
		VertexBufferRHI = RHICmdList.CreateVertexBuffer(PositionData.GetResourceDataSize(), BUF_Static, CreateInfo);
		if (!VertexBufferRHI)
		{
			E_LOG(Error, TEXT("VertexBufferRHI creation failed"));
			return;
		}
	}
};
TGlobalResource<FNDCTriangleVertexBuffer> GNDCTriangleVertexBuffer;

class FPositionUVVertexDeclaration : public FVertexDeclaration
{
public:
	virtual void InitRHI(FRHICommandList& RHICmdList) override
	{
		FVertexDeclarationElementList Elements;
		const uint16 Stride = sizeof(FPositionUV);
		Elements.push_back(FVertexElement(0, STRUCT_OFFSET(FPositionUV, Position), VET_Float3, 0, Stride));
		Elements.push_back(FVertexElement(0, STRUCT_OFFSET(FPositionUV, UV), VET_Float2, 1, Stride));
		VertexDeclarationRHI = GDynamicRHI->RHICreateVertexDeclaration(Elements);
	}
};
TGlobalResource<FPositionUVVertexDeclaration> GPositionUVVertexDeclaration;

class FNDCSquareVertexBuffer : public FVertexBuffer
{
public:
	virtual void InitRHI(FRHICommandList& RHICmdList) override
	{
		TResourceArray<FPositionUV> VertexData;
		// 첫 번째 삼각형 (왼쪽 아래, 오른쪽 아래, 왼쪽 위)
		VertexData.emplace_back(FVector3D(-1.0f, -1.0f, 0.0f), FVector2D(0.0f, 1.0f)); // Bottom left
		VertexData.emplace_back(FVector3D(1.0f, -1.0f, 0.0f), FVector2D(1.0f, 1.0f));  // Bottom right
		VertexData.emplace_back(FVector3D(-1.0f, 1.0f, 0.0f), FVector2D(0.0f, 0.0f));  // Top left

		// 두 번째 삼각형 (오른쪽 아래, 오른쪽 위, 왼쪽 위)
		VertexData.emplace_back(FVector3D(1.0f, -1.0f, 0.0f), FVector2D(1.0f, 1.0f));  // Bottom right
		VertexData.emplace_back(FVector3D(1.0f, 1.0f, 0.0f), FVector2D(1.0f, 0.0f));   // Top right
		VertexData.emplace_back(FVector3D(-1.0f, 1.0f, 0.0f), FVector2D(0.0f, 0.0f));  // Top left

		FRHIResourceCreateInfo CreateInfo(TEXT("MyQuadVertexBuffer"), &VertexData);
		VertexBufferRHI = RHICmdList.CreateVertexBuffer(VertexData.GetResourceDataSize(), BUF_Static, CreateInfo);
		if (!VertexBufferRHI)
		{
			E_LOG(Error, TEXT("VertexBufferRHI creation failed"));
			return;
		}
	}
};
TGlobalResource<FNDCSquareVertexBuffer> GNDCSquareVertexBuffer;


class FFXAAVS : public FShader
{
	DECLARE_SHADER_TYPE(FFXAAVS)
};
IMPLEMENT_SHADER_TYPE(FFXAAVS, FPaths::ShaderDir() + L"/FXAA.hlsl", "VS", SF_Vertex)

class FFXAAPS : public FShader
{
	DECLARE_SHADER_TYPE(FFXAAPS)
};
IMPLEMENT_SHADER_TYPE(FFXAAPS, FPaths::ShaderDir() + L"/FXAA.hlsl", "PS", SF_Pixel)

void FSceneRenderer::Render()
{
	FSceneTextures::InitializeViewFamily(ViewFamily);
	FSceneTextures& SceneTextures = GetActiveSceneTextures();

	if (ViewFamily.GetShadingPath() == EShadingPath::Forward)
	{
		FRHITexture* RenderTargets[1] = { SceneTextures.Color.Target };
		FRHIRenderPassInfo RPInfo(ARRAYSIZE(RenderTargets), RenderTargets, ERenderTargetActions::Clear_Store, SceneTextures.Depth.Target, EDepthStencilTargetActions::ClearDepthStencil_StoreDepthStencil);

		GetCommandList().BeginRenderPass(RPInfo, TEXT("BasePass"));
		RenderLight();
		RenderMesh();
		GetCommandList().EndRenderPass();
	}
	else if(ViewFamily.GetShadingPath() == EShadingPath::Deferred)
	{
		{
			FRHITexture* RenderTargets[4] = { SceneTextures.GBufferA, SceneTextures.GBufferB, SceneTextures.GBufferC, SceneTextures.GBufferD };
			FRHIRenderPassInfo RPInfo(ARRAYSIZE(RenderTargets), RenderTargets, ERenderTargetActions::Clear_Store, SceneTextures.Depth.Target, EDepthStencilTargetActions::ClearDepthStencil_StoreDepthStencil);
			GetCommandList().BeginRenderPass(RPInfo, TEXT("BasePass"));
			RenderMesh();
			GetCommandList().EndRenderPass();
		}

		{
			RenderLight();
		}
	}
	else
	{
		_ASSERT(false);
	}

	{
		GetCommandList().BeginDrawingViewport(ViewFamily.RenderTarget, FTextureRHIRef());

		FRHIDepthStencilState* DepthStencilState = TStaticDepthStencilState<false>().GetRHI();
		GetCommandList().SetDepthStencilState(DepthStencilState);

		// PostProcess
		{
			// FXAA
			FConfigFile& ConfigFile = GConfig->GetConfig(GEngineIni);
			bool bFXAA = false;
			ConfigFile.Get("/Script/Engine.RendererSettings", "FXAA", bFXAA);

			if (bFXAA /*|| GetAsyncKeyState(VK_F7) & 0x8000*/)
			{
				TShaderMapRef<FFXAAPS> PixelShader;
				const FConstantBufferInfo& ConstantBufferInfo = PixelShader->GetConstantBufferInfo(TEXT("FFXAAUniformBuffer"));
				FXAAUniformBuffer.ScreenSize = ViewFamily.ViewportSize;
				FXAAUniformBufferRHI = RHICreateUniformBuffer(ConstantBufferInfo, &FXAAUniformBuffer, sizeof(FXAAUniformBuffer));
				GetCommandList().SetShaderUniformBuffer(EShaderFrequency::SF_Pixel, FXAAUniformBufferRHI);

				TArray<FRHIShaderParameterResource> Parameters;
				Parameters.emplace_back(TStaticSamplerState<SF_Trilinear, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI(), 0);
				Parameters.emplace_back(SceneTextures.Color.Target, 0);
				GetCommandList().SetShaderParameters(PixelShader.GetPixelShader(), Parameters);

				TShaderMapRef<FFXAAVS> VertextShader;
				DrawRectangle(VertextShader.GetVertexShader(), PixelShader.GetPixelShader());
			}
			else
			{
				GetCommandList().CopyTexture(SceneTextures.Color.Target, ViewFamily.RenderTarget->GetRenderTarget(), FRHICopyTextureInfo());
			}
		}

		// UI Render
		{
			TShaderMapRef<FTestVS> VertextShader;
			TShaderMapRef<FTestPS> PixelShader;
			GetCommandList().SetBoundShaderState(
				GDynamicRHI->RHICreateBoundShaderState(
					GPositionVertexDeclaration.VertexDeclarationRHI,
					VertextShader.GetVertexShader(),
					PixelShader.GetPixelShader()
				).GetReference()
			);

			FRHIRasterizerState* RHIRasterizerState = TStaticRasterizerState<FM_Solid, CM_CW>::GetRHI();
			GetCommandList().SetRasterizerState(RHIRasterizerState);

			GetCommandList().SetPrimitiveTopology(EPrimitiveType::PT_TriangleList);
			GetCommandList().SetStreamSource(0, GNDCTriangleVertexBuffer.VertexBufferRHI, 0);
			GetCommandList().DrawPrimitive(0, 1, 1);
		}

		GetCommandList().EndDrawingViewport(ViewFamily.RenderTarget, true, false);
	}
}

class FDeferredLightVS : public FShader
{
	DECLARE_SHADER_TYPE(FDeferredLightVS)
};

class FDeferredLightPS : public FShader
{
	DECLARE_SHADER_TYPE(FDeferredLightPS)
};
IMPLEMENT_SHADER_TYPE(FDeferredLightVS, FPaths::ShaderDir() + L"/DeferredLightShader.hlsl", "VS", SF_Vertex)
IMPLEMENT_SHADER_TYPE(FDeferredLightPS, FPaths::ShaderDir() + L"/DeferredLightShader.hlsl", "PS", SF_Pixel)

void FSceneRenderer::RenderLight()
{
	FScene* Scene = (FScene*)ViewFamily.Scene;
	TShaderMapRef<FMaterialPS> MaterialPS;
	TShaderMapRef<FDeferredLightPS> DeferredLightPS;
	FShader* Shader = GetShadingPath() == EShadingPath::Forward ? (FShader*)MaterialPS.GetShader() : (FShader*)DeferredLightPS.GetShader();
	
	const FConstantBufferInfo& ConstantBufferInfo = Shader->GetConstantBufferInfo(TEXT("FLightShaderParameters"));
	FLightShaderParameters DirectionalLightShaderParameters;

	int32 DirectionalLightCount = 0;
	for (FLightSceneProxy* Proxy : Scene->LightSceneProxies)
	{
		TEnginePtr<UDirectionalLightComponent> DirectionalLightComponent = Cast<UDirectionalLightComponent>(Proxy->GetLightComponent());
		if (DirectionalLightComponent)
		{
			if (DirectionalLightCount >= 1)
			{
				E_LOG(Warning, TEXT("UDirectionalLightComponent: {}. 1개만 지원합니다."), DirectionalLightCount);
			}

			++DirectionalLightCount;
			Proxy->GetLightShaderParameters(DirectionalLightShaderParameters);
		}
	}

	FUniformBufferRHIRef LightUniformBufferRHI = RHICreateUniformBuffer(ConstantBufferInfo, &DirectionalLightShaderParameters, sizeof(DirectionalLightShaderParameters));
	GetCommandList().SetShaderUniformBuffer(EShaderFrequency::SF_Pixel, LightUniformBufferRHI);

	if (ViewFamily.GetShadingPath() == EShadingPath::Deferred)
	{
		FSceneTextures& SceneTextures = GetActiveSceneTextures();
		FRHITexture* RenderTargets[1] = { SceneTextures.Color.Target };
		FRHIRenderPassInfo RPInfo(ARRAYSIZE(RenderTargets), RenderTargets, ERenderTargetActions::Clear_Store);
		GetCommandList().BeginRenderPass(RPInfo, TEXT("LightPass"));
		RenderDeferredLight();
		GetCommandList().EndRenderPass();
	}
}

void FSceneRenderer::RenderDeferredLight()
{
	TArray<FRHIShaderParameterResource> Parameters;
	{
		// Sampler
		Parameters.emplace_back(TStaticSamplerState<SF_Point, AM_Clamp, AM_Clamp, AM_Clamp, 0, 16>::GetRHI(), 0); // GBuffer Sampler
		Parameters.emplace_back(TStaticSamplerState<SF_Trilinear, AM_Wrap, AM_Wrap, AM_Wrap, 0, 16>::GetRHI(), 1); // IBL Sampler

		// Textures
		FSceneTextures& SceneTextures = GetActiveSceneTextures();
		Parameters.emplace_back(SceneTextures.GBufferA, 0);
		Parameters.emplace_back(SceneTextures.GBufferB, 1);
		Parameters.emplace_back(SceneTextures.GBufferC, 2);
		Parameters.emplace_back(SceneTextures.GBufferD, 3);

		// CubeTextures
		FTextureRHIRef DiffuseIBL = GetCommandList().GetTexture(FPaths::ContentDir() + L"/Engine/Textures/SunSubMixer_diffuseIBL.dds");
		FTextureRHIRef SpecularIBL = GetCommandList().GetTexture(FPaths::ContentDir() + L"/Engine/Textures/SunSubMixer_specularIBL.dds");
		Parameters.emplace_back(SpecularIBL, 5);
		Parameters.emplace_back(DiffuseIBL, 6);
	}

	TShaderMapRef<FDeferredLightVS> VertexShader;
	TShaderMapRef<FDeferredLightPS> PixelShader;
	GetCommandList().SetShaderParameters(PixelShader.GetPixelShader(), Parameters);
	GetCommandList().SetDepthStencilState(TStaticDepthStencilState<false>().GetRHI());
	DrawRectangle(VertexShader.GetVertexShader(), PixelShader.GetPixelShader());
}

void FSceneRenderer::RenderMesh()
{
	FRHIDepthStencilState* DepthStencilState = TStaticDepthStencilState<true>().GetRHI();
	GetCommandList().SetDepthStencilState(DepthStencilState);

	FScene* Scene = (FScene*)ViewFamily.Scene;

	FShader* VertexShader = nullptr;
	TShaderMapRef<FMaterialVS> MaterialVS;
	TShaderMapRef<FMaterialPS> MaterialPS;
	VertexShader = MaterialVS.GetShader();

	TArray<FStaticMeshDrawCommand> MeshDrawCommands;
	MeshDrawCommands.reserve(Scene->PrimitiveSceneProxies.size());

	FTextureRHIRef DiffuseIBL = GetCommandList().GetTexture(FPaths::ContentDir() + L"/Engine/Textures/SunSubMixer_diffuseIBL.dds");
	FTextureRHIRef SpecularIBL = GetCommandList().GetTexture(FPaths::ContentDir() + L"/Engine/Textures/SunSubMixer_specularIBL.dds");
	FRHISamplerState* IBLSamplerState = TStaticSamplerState<SF_Trilinear, AM_Wrap, AM_Wrap, AM_Wrap, 0, 16>::GetRHI();
	
	const bool bPIE = GEngine->IsPIE();

	for (FPrimitiveSceneProxy* Proxy : Scene->PrimitiveSceneProxies)
	{
		UPrimitiveComponent* PrimitiveComponent = Proxy->GetPrimitiveComponent();
		if (bPIE && PrimitiveComponent->GetOwner()->IsHiddenInGame()) { continue; }
		PrimitiveComponent->UpdateComponentToWorld();

		if (UStaticMeshComponent* StaticMeshComponent = dynamic_cast<UStaticMeshComponent*>(PrimitiveComponent))
		{
			TEnginePtr<UStaticMesh> StaticMesh = StaticMeshComponent->GetStaticMesh();
			TArray<FStaticMeshRenderData>& RenderDatas = StaticMesh->GetRenderData();
			TArray<TEnginePtr<UMaterial>>& OverrideMaterials = StaticMeshComponent->GetOverrideMaterials();
			FMatrix RenderMatrix = StaticMeshComponent->GetRenderMatrix();
			Proxy->SetTransform(RenderMatrix);
			MeshDrawCommands.emplace_back(Proxy, RenderDatas, OverrideMaterials);
		}
	}

	{
		const FConstantBufferInfo& ConstantBufferInfo = VertexShader->GetConstantBufferInfo(TEXT("FSceneUniformBuffer"));
		SceneUniformBuffer.NumRadianceMipLevels = SpecularIBL->GetDesc().NumMips;
		SceneUniformBuffer.EyePosition = ViewFamily.EyePosition;
		SceneUniformBuffer.ViewMatrix = ViewFamily.ViewMatrix.Transpose();
		SceneUniformBuffer.ProjectionMatrix = ViewFamily.ProjectionMatrix.Transpose();
		SceneUniformBuffer.ViewProjectionMatrix = ViewFamily.ViewProjectionMatrix.Transpose();
		SceneUniformBufferRHI = RHICreateUniformBuffer(ConstantBufferInfo, &SceneUniformBuffer, sizeof(SceneUniformBuffer));
		GetCommandList().SetShaderUniformBuffer(EShaderFrequency::SF_Vertex, SceneUniformBufferRHI);
		GetCommandList().SetShaderUniformBuffer(EShaderFrequency::SF_Pixel, SceneUniformBufferRHI);

		for (const FStaticMeshDrawCommand& StaticMeshDrawCommand : MeshDrawCommands)
		{
			int Index = 0;
			for (FStaticMeshRenderData& RenderData : StaticMeshDrawCommand.RenderDatas)
			{
				UMaterial* Material = RenderData.Material;
				if (StaticMeshDrawCommand.OverrideMaterials[Index])
				{
					Material = StaticMeshDrawCommand.OverrideMaterials[Index];
				}
				TArray<FRHIShaderParameterResource> Parameters;
				
				{
					// Sampler
					Parameters.emplace_back(Material->GetTextureSampler(), 0);

					// Textures
					Parameters.emplace_back(Material->GetBaseColorTextureRHI(), 0);
					Parameters.emplace_back(Material->GetNormalTextureRHI(), 1);
					Parameters.emplace_back(Material->GetMetallicTextureRHI(), 2);
					Parameters.emplace_back(Material->GetRoughnessTextureRHI(), 3);
					Parameters.emplace_back(Material->GetAmbientOcclusionTextureRHI(), 4);
				}

				// CubeTextutes, IBL Sampler
				if (ViewFamily.GetShadingPath() == EShadingPath::Forward)
				{
					Parameters.emplace_back(IBLSamplerState, 1);
					Parameters.emplace_back(SpecularIBL, 5);
					Parameters.emplace_back(DiffuseIBL, 6);
				}

				GetCommandList().SetShaderParameters(Material->GetPixelShaderRHI(), Parameters);
				GetCommandList().SetBoundShaderState(
					GDynamicRHI->RHICreateBoundShaderState(
						RenderData.VertexFactory.GetDeclaration(),
						Material->GetVertexShaderRHI(),
						Material->GetPixelShaderRHI()
					).GetReference()
				);

				FObjectUniformBuffer ObjectUniformBuffer;
				ObjectUniformBuffer.WorldMatrix = StaticMeshDrawCommand.Proxy->GetTransform();
				RenderData.VertexFactory.UpdateObjectUniformBuffer(GetCommandList(), ObjectUniformBuffer);
				GetCommandList().SetPrimitiveTopology(EPrimitiveType::PT_TriangleList);
				GetCommandList().SetRasterizerState(Material->GetRasterizerState());
				GetCommandList().SetStreamSource(0, RenderData.VertexFactory.GetVertexBufferRHI(), 0);
				GetCommandList().DrawIndexedPrimitive(RenderData.VertexFactory.GetIndexBufferRHI(), 0, 0,
					RenderData.NumVertices, 0, RenderData.NumPrimitives, 1);

				++Index;
			}
		}
	}
}

void FSceneRenderer::DrawRectangle(FRHIVertexShader* VertexShader, FRHIPixelShader* PixelShader)
{
	FRHIRasterizerState* RHIRasterizerState = TStaticRasterizerState<FM_Solid, CM_CW>::GetRHI();
	GetCommandList().SetRasterizerState(RHIRasterizerState);
	GetCommandList().SetBoundShaderState(
		GDynamicRHI->RHICreateBoundShaderState(
			GPositionUVVertexDeclaration.VertexDeclarationRHI,
			VertexShader, PixelShader
		).GetReference()
	);
	GetCommandList().SetPrimitiveTopology(EPrimitiveType::PT_TriangleList);
	GetCommandList().SetStreamSource(0, GNDCSquareVertexBuffer.VertexBufferRHI, 0);
	GetCommandList().DrawPrimitive(0, 2, 1);
}

FViewFamilyInfo::FViewFamilyInfo(const FSceneViewFamily& InViewFamily)
	: FSceneViewFamily(InViewFamily)
{
	bIsViewFamilyInfo = true;

	ShadingPath = ::GetShadingPath();
}
