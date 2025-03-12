#pragma once
#include "CoreTypes.h"

#define UE_SMALL_NUMBER			(1.e-8f)


class FMath
{
public:
    static bool IsNearlyEqual(float a, float b, float epsilon = UE_SMALL_NUMBER)
    {
        return std::fabs(a - b) <= epsilon * max(std::fabs(a), std::fabs(b));
    }

    static float Fmod(float X, float Y)
    {
        const float AbsY = std::abs(Y);
        if (AbsY <= UE_SMALL_NUMBER) // Note: this constant should match that used by VectorMod() implementations
        {
            _ASSERT(false);
            return 0.0;
        }

        return fmodf(X, Y);
    }

    template <typename T>
    [[nodiscard]] static constexpr FORCEINLINE T Modulo(T Value, T Base)
    {
        if constexpr (std::is_floating_point_v<T>)
        {
            return FMath::Fmod(Value, Base);
        }
        else
        {
            return Value % Base;
        }
    }

    template<class T>
    static constexpr FORCEINLINE T Wrap(const T X, const T Min, const T Max)
    {
        T Size = Max - Min;
        if (Size == 0)
        {
            return Max;
        }

        T EndVal = X;
        if (EndVal < Min)
        {
            T Mod = FMath::Modulo(Min - EndVal, Size);
            EndVal = (Mod != (T)0) ? Max - Mod : Min;
        }
        else if (EndVal > Max)
        {
            T Mod = FMath::Modulo(EndVal - Max, Size);
            EndVal = (Mod != (T)0) ? Min + Mod : Max;
        }
        return EndVal;
    }

    /**
     * 비교 값을 기준으로 값을 반환합니다. 이 함수의 주요 목적은 컴파일러 내재 함수(intrinsics)를 통해
     * 부동 소수점 비교 기반의 분기를 피하는 것입니다.
     *
     * NaN의 경우 플랫폼별로 차이가 있을 수 있으므로, 이에 대해서는 정의하지 않았습니다.
     *
     * @param   Comparand       비교 값을 기준으로 한 값
     * @param   ValueGEZero     Comparand가 0보다 크거나 같을 때 반환할 값
     * @param   ValueLTZero     Comparand가 0보다 작을 때 반환할 값
     *
     * @return  Comparand가 0보다 크거나 같으면 ValueGEZero, 그렇지 않으면 ValueLTZero를 반환합니다.
     */
    static constexpr FORCEINLINE float FloatSelect(float Comparand, float ValueGEZero, float ValueLTZero)
    {
        return Comparand >= 0.f ? ValueGEZero : ValueLTZero;
    }

    /**
     * 비교 값을 기준으로 값을 반환합니다. 이 함수의 주요 목적은 컴파일러 내재 함수(intrinsics)를 통해
     * 부동 소수점 비교 기반의 분기를 피하는 것입니다.
     *
     * NaN의 경우 플랫폼별로 차이가 있을 수 있으므로, 이에 대해서는 정의하지 않았습니다.
     *
     * @param   Comparand       비교 값을 기준으로 한 값
     * @param   ValueGEZero     Comparand가 0보다 크거나 같을 때 반환할 값
     * @param   ValueLTZero     Comparand가 0보다 작을 때 반환할 값
     *
     * @return  Comparand가 0보다 크거나 같으면 ValueGEZero, 그렇지 않으면 ValueLTZero를 반환합니다.
     */
    static constexpr FORCEINLINE double FloatSelect(double Comparand, double ValueGEZero, double ValueLTZero)
    {
        return Comparand >= 0.0 ? ValueGEZero : ValueLTZero;
    }

    /** 일반적인 방식으로 절대 값을 계산합니다 */
    template< class T >
    static constexpr FORCEINLINE T Abs(const T A)
    {
        return (A < (T)0) ? -A : A;
    }

    /** T와 0의 관계에 따라 1, 0 또는 -1을 반환합니다 */
    template< class T >
    static constexpr FORCEINLINE T Sign(const T A)
    {
        return (A > (T)0) ? (T)1 : ((A < (T)0) ? (T)-1 : (T)0);
    }

    /** 일반적인 방식으로 더 큰 값을 반환합니다 */
    template< class T >
    static constexpr FORCEINLINE T Max(const T A, const T B)
    {
        return (B < A) ? A : B;
    }

    /** 일반적인 방식으로 더 작은 값을 반환합니다 */
    template< class T >
    static constexpr FORCEINLINE T Min(const T A, const T B)
    {
        return (A < B) ? A : B;
    }

    /** Clamps X to be between Min and Max, inclusive */
    template< class T >
    [[nodiscard]] static constexpr FORCEINLINE T Clamp(const T X, const T MinValue, const T MaxValue)
    {
        return Max(Min(X, MaxValue), MinValue);
    }
    /** Allow mixing float/double arguments, promoting to highest precision type. */
    //MIX_FLOATS_3_ARGS(Clamp);

    /** Clamps X to be between Min and Max, inclusive. Explicitly defined here for floats/doubles because static analysis gets confused between template and int versions. */
    [[nodiscard]] static constexpr FORCEINLINE float Clamp(const float X, const float Min, const float Max) { return Clamp<float>(X, Min, Max); }
    [[nodiscard]] static constexpr FORCEINLINE double Clamp(const double X, const double Min, const double Max) { return Clamp<double>(X, Min, Max); }

    /** Clamps X to be between Min and Max, inclusive. Overload to support mixed int64/int32 types. */
    [[nodiscard]] static constexpr FORCEINLINE int64 Clamp(const int64 X, const int32 Min, const int32 Max) { return Clamp<int64>(X, Min, Max); }

    static CORE_API float ClampAngle(float AngleDegrees, float MinAngleDegrees, float MaxAngleDegrees);
};