#include "Math/Transform.h"

CORE_API const FTransform FTransform::Identity;


FTransform::FTransform(const FQuat& InQuat, FVector3D InTranslation, FVector3D InScale3D)
    : Rotation(InQuat.Rotator()), Translation(InTranslation), Scale3D(InScale3D)
{
}


FTransform::FTransform(const FRotator& InRotator, FVector3D InTranslation, FVector3D InScale3D)
    : Rotation(InRotator), Translation(InTranslation), Scale3D(InScale3D)
{
}

FMatrix FTransform::GetMatrix() const
{
    return FMatrix::CreateScale(Scale3D) *
        FMatrix::CreateFromQuaternion(Rotation.Quaternion()) *
        FMatrix::CreateTranslation(Translation);
}

FMatrix FTransform::GetMatrixNoScale() const
{
    return FMatrix::CreateFromQuaternion(Rotation.Quaternion()) *
        FMatrix::CreateTranslation(Translation);
}

FMatrix FTransform::GetRotationMatrix() const
{
    return FMatrix::CreateFromQuaternion(Rotation.Quaternion());
}

FMatrix FTransform::ToMatrix() const
{
    return GetMatrix();
}

FMatrix FTransform::ToMatrixNoScale() const
{
    return GetMatrixNoScale();
}

FMatrix FTransform::ToRotationMatrix() const
{
    return GetRotationMatrix();
}

FVector3D FTransform::GetSafeScaleReciprocal(const FVector3D& InScale, float Tolerance) const
{
    FVector3D SafeRecipScale;
    SafeRecipScale.x = (std::abs(InScale.x) > Tolerance) ? 1.0f / InScale.x : 0.0f;
    SafeRecipScale.y = (std::abs(InScale.y) > Tolerance) ? 1.0f / InScale.y : 0.0f;
    SafeRecipScale.z = (std::abs(InScale.z) > Tolerance) ? 1.0f / InScale.z : 0.0f;
    return SafeRecipScale;
}

bool FTransform::Equals(const FTransform& Other, float Tolerance) const
{
    return Vector3NearEqual(Other.Translation, Translation, FVector3D(Tolerance, Tolerance, Tolerance)) &&
        RotatorNearEqual(Other.Rotation, Rotation, Tolerance) &&
        Vector3NearEqual(Other.Scale3D, Scale3D, FVector3D(Tolerance, Tolerance, Tolerance));
}
