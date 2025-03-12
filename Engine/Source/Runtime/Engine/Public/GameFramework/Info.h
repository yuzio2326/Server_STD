#pragma once
#include "GameFramework/Actor.h"
#include "Info.generated.h"

/**
 * Info는 세계에서 물리적 표현을 가지지 않도록 설계된 액터의 기본 클래스입니다. 주로 세계에 대한 설정 데이터를 보유하는
 * "관리자" 유형의 클래스에 사용되지만, 복제를 위해 액터가 필요할 수도 있습니다.
 */
UCLASS(/*abstract, hidecategories = (Input, Movement, Collision, Rendering, HLOD, WorldPartition, DataLayers, Transformation), showcategories = ("Input|MouseInput", "Input|TouchInput"), MinimalAPI, NotBlueprintable*/)
class ENGINE_API AInfo : public AActor
{
	GENERATED_BODY()
};