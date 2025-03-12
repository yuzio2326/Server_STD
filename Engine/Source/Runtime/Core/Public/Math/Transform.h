#pragma once
#include "CoreTypes.h"
#include "Rotator.h"
#include "SimpleMath.h"

class CORE_API FTransform
{
public:
    static const FTransform Identity;
    FTransform()
        : Rotation(FRotator::ZeroRotator),
        Translation(FVector3D::Zero),
        Scale3D(FVector3D::One) {
    }

    FTransform(const FQuat& InQuat, FVector3D InTranslation, FVector3D InScale3D);
    FTransform(const FRotator& InRotator, FVector3D InTranslation, FVector3D InScale3D);

    FMatrix GetMatrix() const;
    FMatrix GetMatrixNoScale() const;
    FMatrix GetRotationMatrix() const;

    FMatrix ToMatrix() const;
    FMatrix ToMatrixNoScale() const;
    FMatrix ToRotationMatrix() const;

    FVector3D TransformVectorNoScale(const FVector3D& V) const
    {
        FVector3D RotatedVec = FVector3D::Transform(V, GetRotationMatrix());
        return RotatedVec;
    }

    FORCEINLINE FVector3D GetScaledAxis(EAxis::Type InAxis) const
    {
        if (InAxis == EAxis::X)
        {
            return TransformVectorNoScale(FVector3D(1.f, 0.f, 0.f));
        }
        else if (InAxis == EAxis::Y)
        {
            return TransformVectorNoScale(FVector3D(0.f, 1.f, 0.f));
        }

        return TransformVectorNoScale(FVector3D(0.f, 0.f, 1.f));
    }

    FORCEINLINE FVector3D GetUnitAxis(EAxis::Type InAxis) const
    {
        FVector ScaledAxis = GetScaledAxis(InAxis);
        ScaledAxis.Normalize();
        return ScaledAxis;
    }

    void SetPosition(const FVector3D& NewTranslation) { Translation = NewTranslation; }
    void SetRotation(const FRotator& NewRotation) { Rotation = NewRotation; }
    void SetScale(const FVector3D& NewScale3D) { Scale3D = NewScale3D; }

    FVector3D GetTranslation() const { return Translation; }
    FVector3D GetLocation() const { return Translation; }
    FQuat GetQuaternion() const { return Rotation.Quaternion(); }
    FRotator GetRotation() const { return Rotation; }
    FVector3D GetScale3D() const { return Scale3D; }

    // 상대적인 변환을 계산하는 함수 (확인 필요)
    //FTransform GetRelativeTransform(const FTransform& Other) const
    //{
    //    FTransform Result;

    //    // 상대적 스케일 계산
    //    FVector3D SafeRecipScale3D = GetSafeScaleReciprocal(Other.Scale3D, UE_SMALL_NUMBER);
    //    Result.Scale3D = Scale3D * SafeRecipScale3D;

    //    // Other의 회전이 정규화되어 있는지 확인
    //    //if (Other.Rotation.IsNormalized() == false)
    //    //{
    //    //    return FTransform::Identity;
    //    //}

    //    // 상대적 회전 계산
    //    FQuat Inverse;
    //    Other.Rotation.Quaternion().Inverse(Inverse);
    //    Result.Rotation = (Inverse * Rotation.Quaternion()).Rotator();

    //    // 상대적 위치 계산
    //    Result.Translation = (Inverse * (Translation - Other.Translation)) * SafeRecipScale3D;

    //    return Result;
    //}

    // 안전한 스케일 역수를 계산하는 함수
    FVector3D GetSafeScaleReciprocal(const FVector3D& InScale, float Tolerance = UE_SMALL_NUMBER) const;

    // Test if all components of the transforms are equal, within a tolerance.
    bool Equals(const FTransform& Other, float Tolerance = UE_SMALL_NUMBER) const;

    // See Unreal TTransform<T>::Multiply
    FTransform operator*(const FTransform& Other) const
    {
        const FQuat QuatA = Rotation.Quaternion();
        const FQuat QuatB = Other.Rotation.Quaternion();
        const FVector3D TranslateA = Translation;
        const FVector3D TranslateB = Other.Translation;
        const FVector3D ScaleA = Scale3D;
        const FVector3D ScaleB = Other.Scale3D;

        // 이 수식은 두 변환의 회전을 결합합니다.
        // 쿼터니언 곱셈을 사용하여 회전을 합성하는데, 이는 두 회전을 하나로 합치는 효과가 있습니다.
        // Other.Rotation.Quaternion()는 Other 변환의 회전을 쿼터니언으로 변환하고, 
        // 현재 변환의 회전 쿼터니언과 곱합니다.이는 회전 변환이 누적되는 방식으로, 
        // 첫 번째 회전을 두 번째 회전으로 회전시키는 것을 의미합니다.
        // RotationResult = B.Rotation * A.Rotation
        const FQuat NewRotation = QuatB * QuatA;

        // 이 수식은 두 변환의 위치를 결합합니다.
        // 먼저, Translation* Other.Scale3D는 현재 변환의 위치를 Other 변환의 스케일로 스케일링합니다.
        // 이는 다른 변환의 크기 변화에 따라 현재 변환의 위치가 조정되도록 합니다.
        // 그런 다음, FVector3D::Transform(Translation * Other.Scale3D, Other.Rotation.Quaternion())는 스케일링된 위치를 Other 변환의 회전을 사용하여 회전시킵니다.
        // 이렇게 하면 Other 변환에 의해 회전된 새로운 위치를 얻을 수 있습니다.
        // 마지막으로, Other.Translation를 더하여 최종 위치를 결정합니다.
        // 이는 Other 변환의 위치를 기준으로 현재 변환의 위치를 조정하는 것입니다.

        // TranslateResult = B.Rotate(B.Scale * A.Translation) + B.Translate
        const FVector3D ScaledTransA = TranslateA * ScaleB;
        const FVector3D RotatedTranslate = FVector3D::Transform(ScaledTransA, QuatB);
        FVector3D NewTranslation = RotatedTranslate + TranslateB;

        // 이 수식은 두 변환의 스케일을 결합합니다.
        // Other.Scale3D와 현재 변환의 Scale3D를 곱하여 최종 스케일을 계산합니다.
        // 이는 두 변환의 크기 변화를 함께 고려하여 새로운 스케일을 결정하는 것입니다.
        FVector3D NewScale3D = Other.Scale3D * Scale3D;

        return FTransform(NewRotation, NewTranslation, NewScale3D);
    }

protected:
    /** 이 Transform의 회전 */
    FRotator Rotation;
    /** 이 변환의 위치, 벡터 형태 */
    FVector3D Translation;
    /** 3D 스케일(항상 로컬 공간에서 적용됨), 벡터 형태 */
    FVector3D Scale3D;
};