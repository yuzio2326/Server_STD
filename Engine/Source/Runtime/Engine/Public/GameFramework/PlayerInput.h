#pragma once
#include "CoreMinimal.h"
#include "PlayerInput.generated.h"


/**
 * 축과 키 사이의 매핑을 정의합니다.
 *
 * @see https://docs.unrealengine.com/latest/INT/Gameplay/Input/index.html
**/
//USTRUCT(BlueprintType)
struct FInputAxisKeyMapping
{
    //GENERATED_USTRUCT_BODY()

    /** 축의 이름, 예: "MoveForward" */
    //UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    FName AxisName;

    /** 축 값을 누적할 때 사용할 곱셈 값 */
    //UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    float Scale;

    /** 바인딩할 키 */
    //UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    FKey Key;

    bool operator==(const FInputAxisKeyMapping& Other) const
    {
        return (AxisName == Other.AxisName
            && Key == Other.Key
            && Scale == Other.Scale);
    }

    /*bool operator<(const FInputAxisKeyMapping& Other) const
    {
        bool bResult = false;
        if (AxisName.LexicalLess(Other.AxisName))
        {
            bResult = true;
        }
        else if (AxisName == Other.AxisName)
        {
            if (Key < Other.Key)
            {
                bResult = true;
            }
            else if (Key == Other.Key)
            {
                bResult = (Scale < Other.Scale);

            }
        }
        return bResult;
    }*/

    FInputAxisKeyMapping(const FName InAxisName = NAME_None, const FKey InKey = FKey::None, const float InScale = 1.f)
        : AxisName(InAxisName)
        , Scale(InScale)
        , Key(InKey)
    {
    }
};


/**
 * 플레이어 입력을 처리하는 PlayerController 내의 객체입니다.
 * 네트워크 게임에서 클라이언트에만 존재합니다.
 *
 * @see https://docs.unrealengine.com/latest/INT/Gameplay/Input/index.html
 */
UCLASS(/*config = Input, transient, MinimalAPI*/)
class ENGINE_API UPlayerInput : public UObject
{
    GENERATED_BODY()

public:
    /** 재맵핑할 수 없는 엔진 정의 축 매핑을 추가합니다. */
    static void AddEngineDefinedAxisMapping(const FInputAxisKeyMapping& AxisMapping);

public:
    UPlayerInput();
    ~UPlayerInput();
    void SetLockMouseMode(bool bMode);

public:
    /** 현재 입력 컴포넌트 스택을 기반으로 프레임의 입력 이벤트를 처리합니다. */
    virtual void ProcessInputStack(const TArray<UInputComponent*>& InputComponentStack, const float DeltaTime, const bool bGamePaused);

    /** 이 객체를 플레이어 컨트롤러로 캐스트하여 반환합니다. 플레이어 컨트롤러가 없는 경우 null일 수 있습니다. */
    //UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Input")
    APlayerController* GetOuterAPlayerController() const;

private:
    DirectX::Mouse::State LastMouseState;
    DirectX::Mouse::State InitialMouseState;

    bool bLockMouse = true;

private:
    /** 엔진 시스템에 의해 정의된 축 매핑으로, 사용자가 다시 매핑할 수 없습니다 */
    static TArray<FInputAxisKeyMapping> EngineDefinedAxisMappings;
};