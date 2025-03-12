#pragma once
#include "CoreTypes.h"
#include "Math/SimpleMath.h"

struct CORE_API FRotator
{
    float Pitch;
    float Yaw;
    float Roll;

    FRotator();

    FRotator(float InPitch, float InYaw, float InRoll);

    // 두 Rotator를 더하기 
    FRotator operator+(const FRotator& Other) const;
    FRotator& operator+=(const FRotator& Other);

    // 두 Rotator를 빼기 
    FRotator operator-(const FRotator& Other) const;
    // Rotator를 스칼라 값으로 곱하기 
    FRotator operator*(float Scalar) const;
    // 같은지 비교하기 
    bool operator==(const FRotator& Other) const;
    bool operator!=(const FRotator& Other) const;

    bool Equals(const FRotator& R, float Tolerance) const;

    /**
     * 각도를 [0, 360) 범위로 클램핑합니다.
     *
     * @param Angle 클램핑할 각도.
     * @return 클램핑된 각도.
     */
    static float ClampAxis(float Angle);

    /**
     * 각도를 (-180, 180] 범위로 클램핑합니다.
     *
     * @param Angle 클램핑할 각도.
     * @return 클램핑된 각도.
     */
    static float NormalizeAxis(float Angle);

    void Normalize();

    // Rotator를 정규화하여 0-360도 사이로 변환 
    FRotator GetNormalized() const;

    FQuat Quaternion() const;

    bool ContainsNaN() const;

    static const FRotator ZeroRotator;
};


static FORCEINLINE bool RotatorNearEqual(const FRotator& A, const FRotator& B, const float Tolerance) noexcept
{
    if (!(abs(A.Pitch - B.Pitch) <= Tolerance)) { return false; }
    if (!(abs(A.Yaw - B.Yaw) <= Tolerance)) { return false; }
    if (!(abs(A.Roll - B.Roll) <= Tolerance)) { return false; }
    return true;
}
