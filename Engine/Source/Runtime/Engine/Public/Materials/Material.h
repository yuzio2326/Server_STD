#pragma once
#include "CoreMinimal.h"
#include "Shader.h"
#include "Engine/Texture2D.h"
#include "Material.generated.h"

class FMaterialVS : public FShader
{
	DECLARE_SHADER_TYPE(FMaterialVS)
};
class FMaterialPS : public FShader
{
	DECLARE_SHADER_TYPE(FMaterialPS)
};

class FMaterialDeferredVS : public FShader
{
	DECLARE_SHADER_TYPE(FMaterialDeferredVS)
};
class FMaterialDeferredPS : public FShader
{
	DECLARE_SHADER_TYPE(FMaterialDeferredPS)
};

/**
 * 재질(Material)은 씬의 시각적 외형을 제어하기 위해 메쉬에 적용할 수 있는 에셋입니다.
 * 씬의 빛이 표면에 닿을 때, 재질의 셰이딩 모델을 사용하여 그 빛이 표면과 상호 작용하는 방식을 계산합니다.
 *
 * 경고: 새로운 재질을 직접 생성하면 셰이더 컴파일 시간이 증가합니다! 기존 재질을 사용하여 재질 인스턴스를 만드는 것을 고려하세요.
 */
UCLASS()
class ENGINE_API UMaterial : public UObject
{
	GENERATED_BODY()

public:
	static inline TEnginePtr<UMaterial> DefaultMaterial;
	static TEnginePtr<UMaterial> CreateMaterial(const FString& MaterialName,
		const FString& BaseColorTexturePath, const FString& MetallicTexturePath,
		const FString& RoughnessTexturePath, const FString& NormalTexturePath, const FString& AmbientOcclusionTexturePath);
public:
	UMaterial();
	~UMaterial();
	virtual void PostInitProperties() override;
	void SetVertexShader(FShader* InShader, FRHIVertexShader* InShaderRHI);
	void SetPixelShader(FShader* InShader, FRHIPixelShader* InShaderRHI);

	FRHIVertexShader* GetVertexShaderRHI() { return VertexShaderRHI; }
	FRHIPixelShader* GetPixelShaderRHI() { return PixelShaderRHI; }
	FShader* GetVertexShader() const { return VertexShader; }
	FShader* GetPixelShader() const { return PixelShader; }

	void SetRasterizerState(const ERasterizerState InRasterizerState);
	void SetRasterizerState(FRHIRasterizerState* InRasterizerState) { RHIRasterizerState = InRasterizerState; }
	FRHIRasterizerState* GetRasterizerState() { return RHIRasterizerState; }

	void SetBaseColorTexture(UTexture2D* InTexture) { BaseColorTexture = InTexture; }
	void SetMetallicTexture(UTexture2D* InTexture) { MetallicTexture = InTexture; }
	void SetRoughnessTexture(UTexture2D* InTexture) { RoughnessTexture = InTexture; }
	void SetNormalTexture(UTexture2D* InTexture) { NormalTexture = InTexture; }
	void SetAmbientOcclusionTexture(UTexture2D* InTexture) { AmbientOcclusionTexture = InTexture; }

	UTexture2D* GetBaseColorTexture() const { return BaseColorTexture; }
	UTexture2D* GetMetallicTexture() const { return MetallicTexture; }
	UTexture2D* GetRoughnessTexture() const { return RoughnessTexture; }
	UTexture2D* GetNormalTexture() const { return NormalTexture; }
	UTexture2D* GetAmbientOcclusionTexture() const { return AmbientOcclusionTexture; }

	FRHITexture* GetBaseColorTextureRHI() const { return BaseColorTexture ? BaseColorTexture->GetTextureRHI() : nullptr; }
	FRHITexture* GetMetallicTextureRHI() const { return MetallicTexture ? MetallicTexture->GetTextureRHI() : nullptr; }
	FRHITexture* GetRoughnessTextureRHI() const { return RoughnessTexture ? RoughnessTexture->GetTextureRHI() : nullptr; }
	FRHITexture* GetNormalTextureRHI() const { return NormalTexture ? NormalTexture->GetTextureRHI() : nullptr; }
	FRHITexture* GetAmbientOcclusionTextureRHI() const { return AmbientOcclusionTexture ? AmbientOcclusionTexture->GetTextureRHI() : nullptr; }

	FRHISamplerState* GetTextureSampler() const { return TextureSampler; }

protected:
	UTexture2D* BaseColorTexture = nullptr;
	UTexture2D* MetallicTexture = nullptr;
	UTexture2D* RoughnessTexture = nullptr;
	UTexture2D* NormalTexture = nullptr;
	UTexture2D* AmbientOcclusionTexture = nullptr;

	FRHISamplerState* TextureSampler = nullptr;

protected:
	FShader* VertexShader;
	FShader* PixelShader;
	FRHIVertexShader* VertexShaderRHI = nullptr;
	FRHIPixelShader* PixelShaderRHI = nullptr;
	FRHIRasterizerState* RHIRasterizerState = nullptr;

	UPROPERTY()
	int RasterizerState = E_SOLID_BACK; // ERasterizerState
};