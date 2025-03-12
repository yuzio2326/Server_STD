#include "Materials/Material.h"
#include "RHIStaticStates.h"
#include "Engine/Texture2D.h"
#include "Engine/AssetManager.h"

IMPLEMENT_SHADER_TYPE(FMaterialVS, FPaths::ShaderDir() + L"/MaterialShader.hlsl", "VS", SF_Vertex)
IMPLEMENT_SHADER_TYPE(FMaterialPS, FPaths::ShaderDir() + L"/MaterialShader.hlsl", "PS", SF_Pixel)

IMPLEMENT_SHADER_TYPE(FMaterialDeferredVS, FPaths::ShaderDir() + L"/DeferredMaterialShader.hlsl", "VS", SF_Vertex)
IMPLEMENT_SHADER_TYPE(FMaterialDeferredPS, FPaths::ShaderDir() + L"/DeferredMaterialShader.hlsl", "PS", SF_Pixel)

map<FString, TObjectPtr<UMaterial>> GMaterials;

TEnginePtr<UMaterial> UMaterial::CreateMaterial(const FString& MaterialName,
	const FString& BaseColorTexturePath, const FString& MetallicTexturePath,
	const FString& RoughnessTexturePath, const FString& NormalTexturePath, const FString& AmbientOcclusionTexturePath)
{
	TEnginePtr<UTexture2D> BaseColor = !BaseColorTexturePath.empty() ? FAssetManager::Get()->LoadAsset<UTexture2D>(FPaths::ContentDir() + BaseColorTexturePath, UTexture2D::StaticClass()->GetName().c_str()) : nullptr;
	TEnginePtr<UTexture2D> Metallic = !MetallicTexturePath.empty() ? FAssetManager::Get()->LoadAsset<UTexture2D>(FPaths::ContentDir() + MetallicTexturePath, UTexture2D::StaticClass()->GetName().c_str()) : nullptr;
	TEnginePtr<UTexture2D> Roughness = !RoughnessTexturePath.empty() ? FAssetManager::Get()->LoadAsset<UTexture2D>(FPaths::ContentDir() + RoughnessTexturePath, UTexture2D::StaticClass()->GetName().c_str()) : nullptr;
	TEnginePtr<UTexture2D> Normal = !NormalTexturePath.empty() ? FAssetManager::Get()->LoadAsset<UTexture2D>(FPaths::ContentDir() + NormalTexturePath, UTexture2D::StaticClass()->GetName().c_str()) : nullptr;
	TEnginePtr<UTexture2D> AmbientOcclusion = !AmbientOcclusionTexturePath.empty() ? FAssetManager::Get()->LoadAsset<UTexture2D>(FPaths::ContentDir() + AmbientOcclusionTexturePath, UTexture2D::StaticClass()->GetName().c_str()) : nullptr;

	TObjectPtr<UMaterial> Material = NewObject<UMaterial>(nullptr, UMaterial::StaticClass(), MaterialName);
	{
		GMaterials.emplace(Material->GetName(), Material);

		if (GetShadingPath() == EShadingPath::Forward)
		{
			TShaderMapRef<FMaterialVS> VertexShader;
			TShaderMapRef<FMaterialPS> PixelShader;

			Material->SetVertexShader(VertexShader.GetShader(), VertexShader.GetVertexShader());
			Material->SetPixelShader(PixelShader.GetShader(), PixelShader.GetPixelShader());
		}
		else if (GetShadingPath() == EShadingPath::Deferred)
		{
			TShaderMapRef<FMaterialDeferredVS> VertexShader;
			TShaderMapRef<FMaterialDeferredPS> PixelShader;

			Material->SetVertexShader(VertexShader.GetShader(), VertexShader.GetVertexShader());
			Material->SetPixelShader(PixelShader.GetShader(), PixelShader.GetPixelShader());
		}
		else { _ASSERT(false); }

		Material->SetBaseColorTexture(BaseColor);
		Material->SetMetallicTexture(Metallic);
		Material->SetRoughnessTexture(Roughness);
		Material->SetNormalTexture(Normal);
		Material->SetAmbientOcclusionTexture(AmbientOcclusion);
	}

	return Material;
}

UMaterial::UMaterial()
{
#if !SERVER
	if (HasAnyFlags(EObjectFlags::RF_ClassDefaultObject))
	{
		DefaultMaterial = CreateMaterial
		(
			TEXT("Default Material"),
			TEXT("/Engine/Textures/black-white-tile-ue/black-white-tile_albedo.dds"),
			TEXT("/Engine/Textures/BlackTexture.png"),
			TEXT("/Engine/Textures/black-white-tile-ue/black-white-tile_roughness.dds"),
			TEXT("/Engine/Textures/black-white-tile-ue/black-white-tile_normal-dx.dds"),
			TEXT("/Engine/Textures/black-white-tile-ue/black-white-tile_ao.dds")
		);

		CreateMaterial
		(
			TEXT("DarkWoodStain Material"),
			TEXT("/Engine/Textures/dark-wood-stain-ue/dark-wood-stain_albedo.dds"),
			TEXT("/Engine/Textures/BlackTexture.png"),
			TEXT("/Engine/Textures/dark-wood-stain-ue/dark-wood-stain_roughness.dds"),
			TEXT("/Engine/Textures/dark-wood-stain-ue/dark-wood-stain_normal-dx.dds"),
			TEXT("/Engine/Textures/WhiteTexture.png")
		);

		CreateMaterial
		(
			TEXT("Silver Material"),
			TEXT("/Engine/Textures/silver-ue/silver_albedo.dds"),
			TEXT("/Engine/Textures/silver-ue/silver_metallic.dds"),
			TEXT("/Engine/Textures/silver-ue/silver_roughness.dds"),
			TEXT("/Engine/Textures/silver-ue/silver_normal-dx.dds"),
			TEXT("/Engine/Textures/WhiteTexture.png")
		);

		CreateMaterial
		(
			TEXT("DullCopper Material"),
			TEXT("/Engine/Textures/dull-copper-ue/dull-copper_albedo.dds"),
			TEXT("/Engine/Textures/dull-copper-ue/dull-copper_metallic.dds"),
			TEXT("/Engine/Textures/dull-copper-ue/dull-copper_roughness.dds"),
			TEXT("/Engine/Textures/dull-copper-ue/dull-copper_normal-dx.dds"),
			TEXT("/Engine/Textures/WhiteTexture.png")
		);

		CreateMaterial
		(
			TEXT("Earth Material"),
			TEXT("/Engine/Textures/Earth/8k_earth_daymap.dds"),
			TEXT("/Engine/Textures/Earth/8k_earth_specular_map.dds"),
			TEXT("/Engine/Textures/Earth/8k_earth_specular_map.dds"),
			TEXT("/Engine/Textures/Earth/8k_earth_normal_map.dds"),
			TEXT("/Engine/Textures/WhiteTexture.png")
		);

		CreateMaterial
		(
			TEXT("bamboo-wood-semigloss Material"),
			TEXT("/Engine/Textures/bamboo-wood-semigloss/bamboo-wood-semigloss-albedo.dds"),
			TEXT("/Engine/Textures/BlackTexture.png"),
			TEXT("/Engine/Textures/bamboo-wood-semigloss/bamboo-wood-semigloss-roughness.dds"),
			TEXT("/Engine/Textures/bamboo-wood-semigloss/bamboo-wood-semigloss-normal.dds"),
			TEXT("/Engine/Textures/WhiteTexture.png")
		);
	}

	SetRasterizerState((ERasterizerState)RasterizerState);
	TextureSampler = TStaticSamplerState<SF_Trilinear, AM_Wrap, AM_Wrap, AM_Wrap, 0, 16>::GetRHI();
#endif
}

UMaterial::~UMaterial()
{
	if (HasAnyFlags(EObjectFlags::RF_ClassDefaultObject))
	{
		DefaultMaterial = nullptr;
		GMaterials.clear();
	}
}

void UMaterial::PostInitProperties()
{
	Super::PostInitProperties();

#if !SERVER
	SetRasterizerState((ERasterizerState)RasterizerState);
#endif
}

void UMaterial::SetVertexShader(FShader* InShader, FRHIVertexShader* InShaderRHI)
{
	_ASSERT(InShader);
	_ASSERT(InShaderRHI);
	VertexShader = InShader;
	VertexShaderRHI = InShaderRHI;
}

void UMaterial::SetPixelShader(FShader* InShader, FRHIPixelShader* InShaderRHI)
{
	_ASSERT(InShader);
	_ASSERT(InShaderRHI);
	PixelShader = InShader;
	PixelShaderRHI = InShaderRHI;
}

void UMaterial::SetRasterizerState(const ERasterizerState InRasterizerState)
{
	RasterizerState = InRasterizerState;

	switch (RasterizerState)
	{
	case ERasterizerState::E_SOLID_FRONT:
		RHIRasterizerState = TStaticRasterizerState<FM_Solid, CM_CCW>::GetRHI();
		break;
	case ERasterizerState::E_SOLID_BACK:
		RHIRasterizerState = TStaticRasterizerState<FM_Solid, CM_CW>::GetRHI();
		break;
	case ERasterizerState::E_SOLID_NONE:
		RHIRasterizerState = TStaticRasterizerState<FM_Solid, CM_None>::GetRHI();
		break;

	case ERasterizerState::E_WIREFRAME_FRONT:
		RHIRasterizerState = TStaticRasterizerState<FM_Wireframe, CM_CCW>::GetRHI();
		break;
	case ERasterizerState::E_WIREFRAME_BACK:
		RHIRasterizerState = TStaticRasterizerState<FM_Wireframe, CM_CW>::GetRHI();
		break;
	case ERasterizerState::E_WIREFRAME_NONE:
		RHIRasterizerState = TStaticRasterizerState<FM_Wireframe, CM_None>::GetRHI();
		break;
	default:
		_ASSERT(false);
		RHIRasterizerState = TStaticRasterizerState<FM_Solid, CM_None>::GetRHI();
		break;
	}
}

