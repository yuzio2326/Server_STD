#include "Math/Rotator.h"
#include "Math/MathUtility.h"

const FRotator FRotator::ZeroRotator = FRotator(0.f, 0.f, 0.f);

FRotator::FRotator()
	: Pitch(0.0f), Yaw(0.0f), Roll(0.0f)
{
}

FRotator::FRotator(float InPitch, float InYaw, float InRoll)
	: Pitch(InPitch), Yaw(InYaw), Roll(InRoll) 
{
}

FRotator FRotator::operator+(const FRotator& Other) const
{
    return FRotator(Pitch + Other.Pitch, Yaw + Other.Yaw, Roll + Other.Roll);
}

FRotator& FRotator::operator+=(const FRotator& Other)
{
    Pitch += Other.Pitch;
    Yaw += Other.Yaw;
    Roll += Other.Roll;
    return *this;
}

FRotator FRotator::operator-(const FRotator& Other) const
{
    return FRotator(Pitch - Other.Pitch, Yaw - Other.Yaw, Roll - Other.Roll);
}

FRotator FRotator::operator*(float Scalar) const
{
    return FRotator(Pitch * Scalar, Yaw * Scalar, Roll * Scalar);
}

bool FRotator::operator==(const FRotator& Other) const
{
    return Equals(Other, UE_SMALL_NUMBER);
}

bool FRotator::operator!=(const FRotator& Other) const
{
    return !(*this == Other);
}

bool FRotator::Equals(const FRotator& R, float Tolerance) const
{
    return (abs(NormalizeAxis(Pitch - R.Pitch)) <= Tolerance)
        && (abs(NormalizeAxis(Yaw - R.Yaw)) <= Tolerance)
        && (abs(NormalizeAxis(Roll - R.Roll)) <= Tolerance);
}

float FRotator::ClampAxis(float Angle)
{
    // 각도를 (-360,360) 범위로 변환
    Angle = FMath::Fmod(Angle, 360.f);

    if (Angle < 0.f)
    {
        // [0,360) 범위로 이동
        Angle += 360.f;
    }

    return Angle;
}

float FRotator::NormalizeAxis(float Angle)
{
    // returns Angle in the range [0,360)
    Angle = ClampAxis(Angle);

    if (Angle > 180.f)
    {
        // shift to (-180,180]
        Angle -= 360.f;
    }

    return Angle;
}

void FRotator::Normalize()
{
    Pitch = NormalizeAxis(Pitch);
    Yaw = NormalizeAxis(Yaw);
    Roll = NormalizeAxis(Roll);
}

FRotator FRotator::GetNormalized() const
{
    FRotator Rot = *this;
    Rot.Normalize();
    return Rot;
}

FQuat FRotator::Quaternion() const
{
    // 언리얼이랑 맞추기 위해 변환
    //return FQuat::CreateFromYawPitchRoll(-DirectX::XMConvertToRadians(Roll), DirectX::XMConvertToRadians(Yaw), DirectX::XMConvertToRadians(Pitch));
    //return FQuat::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(Yaw), DirectX::XMConvertToRadians(Pitch), DirectX::XMConvertToRadians(Roll));
    const float DEG_TO_RAD = DirectX::XM_PI / (180.f);
    const float RADS_DIVIDED_BY_2 = DEG_TO_RAD / 2.f;
    float SP, SY, SR;
    float CP, CY, CR;

    const float PitchNoWinding = FMath::Fmod(Pitch, 360.0f);
    const float YawNoWinding = FMath::Fmod(Yaw, 360.0f);
    const float RollNoWinding = FMath::Fmod(Roll, 360.0f);

    SP = sin(PitchNoWinding * RADS_DIVIDED_BY_2);
    CP = cos(PitchNoWinding * RADS_DIVIDED_BY_2);

    SY = sin(YawNoWinding * RADS_DIVIDED_BY_2);
    CY = cos(YawNoWinding * RADS_DIVIDED_BY_2);

    SR = sin(RollNoWinding * RADS_DIVIDED_BY_2);
    CR = cos(RollNoWinding * RADS_DIVIDED_BY_2);
    //FMath::SinCos(&SP, &CP, PitchNoWinding * RADS_DIVIDED_BY_2);
    //FMath::SinCos(&SY, &CY, YawNoWinding * RADS_DIVIDED_BY_2);
    //FMath::SinCos(&SR, &CR, RollNoWinding * RADS_DIVIDED_BY_2);

    FQuat RotationQuat;
    RotationQuat.x = CR * SP * SY - SR * CP * CY;
    RotationQuat.y = -CR * SP * CY - SR * CP * SY;
    RotationQuat.z = CR * CP * SY - SR * SP * CY;
    RotationQuat.w = CR * CP * CY + SR * SP * SY;
    return RotationQuat;
}

bool FRotator::ContainsNaN() const
{
    return (!_finite(Pitch) ||
        !_finite(Yaw) ||
        !_finite(Roll));
}
