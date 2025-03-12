#include "ScenePrivate.h"
#include "EngineModule.h"
#include "RendererInterface.h"
#include "Engine/World.h"

#include "Components/PrimitiveComponent.h"
#include "PrimitiveSceneProxy.h"

#include "Components/DirectionalLightComponent.h"
#include "LightSceneProxy.h"

FScene::FScene(UWorld* InWorld, /*bool bInRequiresHitProxies, bool bInIsEditorScene, bool bCreateFXSystem,*/ ERHIFeatureLevel::Type InFeatureLevel)
	: FSceneInterface(InFeatureLevel)
	, World(InWorld)
{
	_ASSERT(World);
	World->Scene = this;
}

FScene::~FScene()
{
}

void FScene::AddPrimitive(UPrimitiveComponent* Primitive)
{
	FPrimitiveSceneProxy* Proxy = Primitive->CreateSceneProxy();
	if (!Proxy) { return; }

	PrimitiveSceneProxies.push_back(Proxy);
}

void FScene::RemovePrimitive(UPrimitiveComponent* Primitive)
{
	auto It = find_if(PrimitiveSceneProxies.begin(), PrimitiveSceneProxies.end(),
		[Primitive](FPrimitiveSceneProxy* Proxy)
		{
			return Proxy->GetPrimitiveComponent() == Primitive;
		}
	);

	if (It == PrimitiveSceneProxies.end())
	{
		E_LOG(Error, TEXT("Check"));
		return;
	}

	PrimitiveSceneProxies.erase(It);
}

void FScene::AddLight(ULightComponent* Light)
{
	FLightSceneProxy* Proxy = Light->CreateSceneProxy();
	if (!Proxy) { return; }

	LightSceneProxies.push_back(Proxy);
}

void FScene::RemoveLight(ULightComponent* Light)
{
	auto It = find_if(LightSceneProxies.begin(), LightSceneProxies.end(),
		[Light](FLightSceneProxy* Proxy)
		{
			return Proxy->GetLightComponent() == Light;
		}
	);

	if (It == LightSceneProxies.end())
	{
		E_LOG(Error, TEXT("Check"));
		return;
	}

	LightSceneProxies.erase(It);
}

void FScene::Release()
{
	for (FPrimitiveSceneProxy* Proxy : PrimitiveSceneProxies)
	{
		delete Proxy;
	}
	PrimitiveSceneProxies.clear();

	for (FLightSceneProxy* Proxy : LightSceneProxies)
	{
		delete Proxy;
	}
	LightSceneProxies.clear();

	GetRendererModule().RemoveScene(this);
}
