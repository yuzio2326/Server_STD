#pragma once
#include "Components/ActorComponent.h"
#include "InputComponent.generated.h"

/** 다양한 바인딩 유형의 기본 클래스입니다. */
struct FInputBinding
{
    /** 바인딩이 입력을 소비할지 또는 다른 컴포넌트로 전달할지를 나타냅니다. */
    uint8 bConsumeInput : 1;

    /** 바인딩이 일시 중지된 동안에도 실행될지를 나타냅니다. */
    uint8 bExecuteWhenPaused : 1;

    FInputBinding()
        : bConsumeInput(true)
        , bExecuteWhenPaused(false)
    {
    }
};

/** 축 매핑에 델리게이트를 바인딩합니다. */
struct FInputAxisBinding : public FInputBinding
{
    /** 바인딩될 축 매핑입니다. */
    FName AxisName;

    /**
     * 축에 바인딩된 델리게이트입니다.
     * 입력 컴포넌트가 입력 스택에 있는 각 프레임마다 값이 0이거나 변경되었는지와 상관없이 호출됩니다.
     */
    FSafeDelegate<float> AxisDelegate;

    /**
     * 가장 최근의 UPlayerInput::ProcessInputStack 동안 계산된 축의 값입니다.
     * InputComponent가 스택에 없을 경우 모든 값은 0이어야 합니다.
     */
    float AxisValue;

    FInputAxisBinding()
        : FInputBinding()
        , AxisName(NAME_None)
        , AxisValue(0.f)
    {
    }

    FInputAxisBinding(const FName InAxisName)
        : FInputBinding()
        , AxisName(InAxisName)
        , AxisValue(0.f)
    {
    }
};


/**
 * 입력 바인딩을 위한 액터 컴포넌트를 구현합니다.
 *
 * 입력 컴포넌트는 다양한 형태의 입력 이벤트를 대리 함수에 바인딩할 수 있도록 하는 일시적인 컴포넌트입니다.
 * 입력 컴포넌트는 PlayerController가 관리하는 스택에서 처리되고, PlayerInput에 의해 처리됩니다.
 * 각 바인딩은 입력 이벤트를 소비하여 입력 스택의 다른 컴포넌트가 입력을 처리하지 못하게 합니다.
 *
 * @see https://docs.unrealengine.com/latest/INT/Gameplay/Input/index.html
 */
UCLASS(/*NotBlueprintable, transient, config = Input, hidecategories = (Activation, "Components|Activation"), MinimalAPI*/)
class UInputComponent : public UActorComponent
{
	GENERATED_BODY()

public:
    /** 축 바인딩의 모음입니다. */
    TArray<FInputAxisBinding> AxisBindings;

	template<class UserClass>
    FInputAxisBinding& BindAxis(FName AxisName, UserClass* Object, void (UserClass::* Func)(float))
    {
        FInputAxisBinding AB(AxisName);
        AB.AxisDelegate.AddUObject(Object, Func);
        return AxisBindings.emplace_back(AB);
    }
};