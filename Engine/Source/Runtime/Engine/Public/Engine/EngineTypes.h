#pragma once
#include "CoreMinimal.h"

#define LIKELY(x)			(!!(x))

/** UWorld 객체의 goal/source를 지정합니다 */
namespace EWorldType
{
    enum Type
    {
        /** 유형이 지정되지 않은 월드, 대부분의 경우 스트리밍된 서브 레벨의 잔여 월드가 됩니다 */
        None,

        /** 게임 월드 */
        Game,

        /** 에디터에서 편집 중인 월드 */
        Editor,

        /** 에디터 내 플레이(PIE) 월드 */
        PIE,

        /** 에디터 도구용 프리뷰 월드 */
        EditorPreview,

        /** 게임용 프리뷰 월드 */
        GamePreview,

        /** 게임용 최소 RPC 월드 */
        GameRPC,

        /** 레벨 에디터에서 현재 편집 중이 아닌 로드된 에디터 월드 */
        Inactive
    };
}

/** Quat<->Rotator 변환을 캐시하기 위한 구조체입니다. */
struct FRotationConversionCache
{
    FRotationConversionCache()
    {
    }

    /** FRotator를 FQuat로 변환합니다. 가능한 경우 캐시된 변환을 사용하고, 일치하는 항목이 없으면 캐시를 업데이트합니다. */
    FORCEINLINE FQuat RotatorToQuat(const FRotator& InRotator) const
    {
        FPayload& Payload = GetOrCreatePayload();
        if (LIKELY(Payload.CachedRotator != InRotator))
        {
            Payload.CachedRotator = InRotator.GetNormalized();
            Payload.CachedQuat = Payload.CachedRotator.Quaternion();
        }
        return Payload.CachedQuat;
    }

    /** FRotator를 FQuat로 변환합니다. 가능한 경우 캐시된 변환을 사용하지만, 일치하는 항목이 없으면 캐시를 업데이트하지 않습니다. */
    FORCEINLINE FQuat RotatorToQuat_ReadOnly(const FRotator& InRotator) const
    {
        if (PayloadPtr)
        {
            FPayload& Payload = *PayloadPtr;
            if (LIKELY(Payload.CachedRotator == InRotator))
            {
                return Payload.CachedQuat;
            }
        }
        return InRotator.Quaternion();
    }

    /** FQuat를 FRotator로 변환합니다. 가능한 경우 캐시된 변환을 사용하고, 일치하는 항목이 없으면 캐시를 업데이트합니다. */
    //FORCEINLINE FRotator QuatToRotator(const FQuat& InQuat) const
    //{
    //  FPayload& Payload = GetOrCreatePayload();
    //  if (LIKELY(Payload.CachedQuat != InQuat))
    //  {
    //      Payload.CachedQuat = InQuat.GetNormalized();
    //      Payload.CachedRotator = Payload.CachedQuat.Rotator();
    //  }
    //  return Payload.CachedRotator;
    //}

    /** Quat이 이미 정규화된 경우 사용되는 QuatToRotator의 버전입니다. */
    FORCEINLINE FRotator NormalizedQuatToRotator(const FQuat& InNormalizedQuat) const
    {
        FPayload& Payload = GetOrCreatePayload();
        if (LIKELY(Payload.CachedQuat != InNormalizedQuat))
        {
            Payload.CachedQuat = InNormalizedQuat;
            Payload.CachedRotator = InNormalizedQuat.Rotator();
        }
        return Payload.CachedRotator;
    }

private:

    struct FPayload
    {
        mutable FQuat               CachedQuat;     // CachedRotator와 일치하는 FQuat. CachedQuat.Rotator() == CachedRotator 여야 함.
        mutable FRotator            CachedRotator;  // CachedQuat와 일치하는 FRotator. CachedRotator.Quaternion() == CachedQuat 여야 함.

        FPayload()
            : CachedQuat(FQuat::Identity)
            , CachedRotator(FRotator::ZeroRotator)
        {
        }
    };

    inline FPayload& GetOrCreatePayload() const
    {
        if (!PayloadPtr)
        {
            PayloadPtr = make_unique<FPayload>();
        }
        return *PayloadPtr;
    }

    mutable unique_ptr<FPayload>    PayloadPtr;
};
