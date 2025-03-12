#include "Engine/Light.h"
#include "Engine/AssetManager.h"
#include "Components/LightComponent.h"
#include "Components/StaticMeshComponent.h"

ALight::ALight(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	LightComponent = CreateDefaultSubobject<ULightComponent>(TEXT("LightComponent0"));
	RootComponent = LightComponent->As<ULightComponent>();

	TEnginePtr<UStaticMesh> StaticMesh = FAssetManager::Get()->LoadAsset<UStaticMesh>(FPaths::ContentDir() + L"/Engine/Mesh/Cone.FBX");
	LightDirectionMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LightDirectionMesh"));
	LightDirectionMesh->SetStaticMesh(StaticMesh);
	LightDirectionMesh->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));
	LightDirectionMesh->SetRelativeScale3D(FVector(0.3f, 0.3f, 0.7f));

	SetActorHiddenInGame(true);
}

void ALight::SetLightColor(FLinearColor Color)
{
	LightComponent->SetLightColor(Color);
}

void ALight::OnPropertyChanged(FProperty& Property)
{
	Super::OnPropertyChanged(Property);

	FRotator LightRotation = LightComponent->GetRelativeRotation();
	FRotator DefaultRotation = GetDefault<ALight>()->LightDirectionMesh->GetRelativeRotation();
	FRotator NewMeshRotation = LightRotation + DefaultRotation;
	LightDirectionMesh->SetRelativeRotation(FRotator(NewMeshRotation.Pitch, 0.f, NewMeshRotation.Yaw));
	LightDirectionMesh->SetRelativeLocation(LightComponent->GetRelativeLocation());
}
