#include "Math/MathUtility.h"
#include "Math/Rotator.h"

float FMath::ClampAngle(float AngleDegrees, float MinAngleDegrees, float MaxAngleDegrees)
{
    const float MaxDelta = FRotator::ClampAxis(MaxAngleDegrees - MinAngleDegrees) * 0.5f;           // 0..180
    const float RangeCenter = FRotator::ClampAxis(MinAngleDegrees + MaxDelta);                      // 0..360
    const float DeltaFromCenter = FRotator::NormalizeAxis(AngleDegrees - RangeCenter);              // -180..180

    // 가장자리에 클램핑할 수도 있습니다
    if (DeltaFromCenter > MaxDelta)
    {
        return FRotator::NormalizeAxis(RangeCenter + MaxDelta);
    }
    else if (DeltaFromCenter < -MaxDelta)
    {
        return FRotator::NormalizeAxis(RangeCenter - MaxDelta);
    }

    // 이미 범위 내에 있으므로 그대로 반환합니다
    return FRotator::NormalizeAxis(AngleDegrees);
}
