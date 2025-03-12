#include "Engine/StaticMeshActor.h"
#include "Engine/StaticMesh.h"
#include "Engine/AssetManager.h"
#include "RHIStaticStates.h"
#include "Materials/Material.h"

AStaticMeshActor::AStaticMeshActor()
{
	TEnginePtr<UStaticMesh> StaticMesh =
		FAssetManager::Get()->LoadAsset<UStaticMesh>(FPaths::ContentDir() + L"/Engine/Mesh/Cube.FBX");
	FAssetManager::Get()->LoadAsset<UStaticMesh>(FPaths::ContentDir() + L"/Engine/Mesh/Cone.FBX");
	FAssetManager::Get()->LoadAsset<UStaticMesh>(FPaths::ContentDir() + L"/Engine/Mesh/Cylinder.FBX");
	FAssetManager::Get()->LoadAsset<UStaticMesh>(FPaths::ContentDir() + L"/Engine/Mesh/Plane.FBX");
	FAssetManager::Get()->LoadAsset<UStaticMesh>(FPaths::ContentDir() + L"/Engine/Mesh/SK_SMG11_X.FBX");
	FAssetManager::Get()->LoadAsset<UStaticMesh>(FPaths::ContentDir() + L"/Engine/Mesh/SK_KA47_X.FBX");
	FAssetManager::Get()->LoadAsset<UStaticMesh>(FPaths::ContentDir() + L"/Engine/Mesh/Sphere.FBX");

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetStaticMesh(StaticMesh);

	ChildStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChildStaticMeshComponent"));
	ChildStaticMeshComponent->SetStaticMesh(StaticMesh);
	ChildStaticMeshComponent->SetRelativeLocation(FVector(0.f, 0.f, -100.f));
	ChildStaticMeshComponent->SetupAttachment(StaticMeshComponent);

	RootComponent = Cast<USceneComponent>(StaticMeshComponent);
}

void AStaticMeshActor::OnPropertyChanged(FProperty& InProperty)
{
	Super::OnPropertyChanged(InProperty);

	//for (uint32 i = 0; i < StaticMeshComponent->GetMaterialCount(); ++i)
	//{
	//	UMaterial* Material = StaticMeshComponent->GetMaterial(i);
	//	Material->SetRasterizerState((ERasterizerState)RasterizerState);
	//}
}

void AStaticMeshActor::BeginPlay()
{
	Super::BeginPlay();
}

void AStaticMeshActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	FRotator Rotator = ChildStaticMeshComponent->GetRelativeRotation();
	Rotator.Yaw += 360.f * DeltaSeconds;
	Rotator.Normalize();
	ChildStaticMeshComponent->SetRelativeRotation(Rotator);
}
