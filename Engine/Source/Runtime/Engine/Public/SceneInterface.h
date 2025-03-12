#pragma once
#include "CoreMinimal.h"
#include "RHI.h"

class UPrimitiveComponent;
class ULightComponent;

/**
 * 씬의 private scene manager 구현에 대한 인터페이스입니다. GetRendererModule().AllocateScene을 사용하여 생성하세요.
 */
class FSceneInterface
{
    friend class FRendererModule;
public:
	ENGINE_API FSceneInterface(ERHIFeatureLevel::Type InFeatureLevel);

    /**
     * 새로운 프리미티브 컴포넌트를 장면에 추가합니다.
     *
     * @param Primitive - 추가할 프리미티브 컴포넌트
     */
    virtual void AddPrimitive(UPrimitiveComponent* Primitive) = 0;
    /**
     * 프리미티브 컴포넌트를 장면에서 제거합니다.
     *
     * @param Primitive - 제거할 프리미티브 컴포넌트
     */
    virtual void RemovePrimitive(UPrimitiveComponent* Primitive) = 0;

    /**
     * 새로운 조명 컴포넌트를 씬에 추가합니다.
     *
     * @param Light - 추가할 조명 컴포넌트
     */
    virtual void AddLight(ULightComponent* Light) = 0;
    /**
     * 조명 컴포넌트를 씬에서 제거합니다.
     *
     * @param Light - 제거할 조명 컴포넌트
     */
    virtual void RemoveLight(ULightComponent* Light) = 0;


    virtual void Release() {}

protected:
    virtual ~FSceneInterface() {}

    /** 이 씬의 기능 수준 */
    ERHIFeatureLevel::Type FeatureLevel;
};