#include "Components/SceneComponent.h"
#include "GameFramework/Actor.h"

USceneComponent::USceneComponent()
{
}

USceneComponent* USceneComponent::GetAttachParent() const
{
    return AttachParent;
}

FTransform USceneComponent::GetSocketTransform(FName InSocketName, ERelativeTransformSpace TransformSpace) const
{
	switch (TransformSpace)
	{
	case RTS_Actor:
	{
        _ASSERT(false);
		//return GetComponentTransform().GetRelativeTransform(GetOwner()->GetTransform());
        return FTransform::Identity;
	}
	case RTS_Component:
	case RTS_ParentBoneSpace:
	{
		return FTransform::Identity;
	}
	default:
	{
		return GetComponentTransform();
	}
	}
}

void USceneComponent::SetRelativeLocation(FVector NewLocation)
{
    SetRelativeLocationAndRotation(NewLocation, RelativeRotationCache.RotatorToQuat(GetRelativeRotation())/*, bSweep, OutSweepHitResult, Teleport*/);
}

void USceneComponent::SetRelativeRotation(FRotator NewRotation)
{
    if (!NewRotation.Equals(GetRelativeRotation(), SCENECOMPONENT_ROTATOR_TOLERANCE))
    {
        // We know the rotations are different, don't bother with the cache.
        SetRelativeLocationAndRotation(GetRelativeLocation(), NewRotation.Quaternion()/*, bSweep, OutSweepHitResult, Teleport*/);
    }
}

void USceneComponent::SetRelativeRotation(const FQuat& NewRotation)
{
    SetRelativeLocationAndRotation(GetRelativeLocation(), NewRotation/*, bSweep, OutSweepHitResult, Teleport*/);
}

void USceneComponent::SetRelativeTransform(const FTransform& NewTransform)
{
    SetRelativeLocationAndRotation(NewTransform.GetTranslation(), NewTransform.GetRotation()/*, bSweep, OutSweepHitResult, Teleport*/);
    SetRelativeScale3D(NewTransform.GetScale3D());
}

void USceneComponent::SetRelativeLocationAndRotation(FVector NewLocation, FRotator NewRotation)
{
    RelativeLocation = NewLocation;
    RelativeRotation = NewRotation;

    UpdateComponentToWorld();
}

void USceneComponent::SetRelativeLocationAndRotation(FVector NewLocation, const FQuat& NewRotation)
{
    RelativeLocation = NewLocation;
    RelativeRotation = NewRotation.Rotator(); //RelativeRotationCache.QuatToRotator(NewRotation);

    UpdateComponentToWorld();
}

void USceneComponent::SetRelativeScale3D(FVector NewScale3D)
{
    if (NewScale3D != GetRelativeScale3D())
    {
        RelativeScale3D = NewScale3D;

        //if (UNLIKELY(NeedsInitialization() || OwnerNeedsInitialization()))
        //{
        //    // If we're in the component or actor constructor, don't do anything else.
        //    return;
        //}

        UpdateComponentToWorld();

        //if (IsRegistered())
        //{
        //    if (!IsDeferringMovementUpdates())
        //    {
        //        UpdateOverlaps();
        //    }
        //    else
        //    {
        //        // Invalidate cached overlap state at this location.
        //        TArray<FOverlapInfo> EmptyOverlaps;
        //        GetCurrentScopedMovement()->AppendOverlapsAfterMove(EmptyOverlaps, false, false);
        //    }
        //}
    }
}

void USceneComponent::SetWorldTransform(const FTransform& NewTransform)
{
    // 무언가에 붙어 있다면, 로컬 공간으로 변환합니다.
    if (GetAttachParent() != nullptr)
    {
        _ASSERT(false);// 미구현

        //const FTransform ParentToWorld = GetAttachParent()->GetSocketTransform(NAME_None /*GetAttachSocketName()*/);
        //FTransform RelativeTM = NewTransform.GetRelativeTransform(ParentToWorld);

        //// 절대 위치, 회전, 스케일은 월드 변환을 직접 사용합니다.
        //if (IsUsingAbsoluteLocation())
        //{
        //    RelativeTM.CopyTranslation(NewTransform);
        //}

        //if (IsUsingAbsoluteRotation())
        //{
        //    RelativeTM.CopyRotation(NewTransform);
        //}

        //if (IsUsingAbsoluteScale())
        //{
        //    RelativeTM.CopyScale3D(NewTransform);
        //}

        //SetRelativeTransform(RelativeTM, bSweep, OutSweepHitResult, Teleport);
    }
    else
    {
        SetRelativeTransform(NewTransform/*, bSweep, OutSweepHitResult, Teleport*/);
    }
}

void USceneComponent::SetWorldLocation(FVector NewLocation)
{
    FVector NewRelLocation = NewLocation;

    // If attached to something, transform into local space
    if (GetAttachParent() != nullptr /*&& !IsUsingAbsoluteLocation()*/)
    {
        _ASSERT(false); // 미구현
        //FTransform ParentToWorld = GetAttachParent()->GetSocketTransform(GetAttachSocketName());
        //NewRelLocation = ParentToWorld.InverseTransformPosition(NewLocation);
    }

    SetRelativeLocation(NewRelLocation/*, bSweep, OutSweepHitResult, Teleport*/);
}

void USceneComponent::SetupAttachment(USceneComponent* InParent)
{
    if (InParent != AttachParent /*|| InSocketName != AttachSocketName*/)
    {
        // TEXT("SetupAttachment should only be used to initialize AttachParent and AttachSocketName for a future AttachToComponent. Once a component is registered you must use AttachToComponent. Owner [%s], InParent [%s], InSocketName [%s]"), *GetPathNameSafe(GetOwner()), *GetNameSafe(InParent), *InSocketName.ToString()
        _ASSERT(!bRegistered);
        if (!bRegistered)
        {
            _ASSERT(InParent != this, TEXT("Cannot attach a component to itself."));
            if (InParent != this)
            {
                _ASSERT(InParent == nullptr || !InParent->IsAttachedTo(this),
                    TEXT("Setting up attachment would create a cycle."));
                if (InParent == nullptr || !InParent->IsAttachedTo(this))
                {
                    _ASSERT(AttachParent == nullptr || AttachParent->AttachChildren.end() == find(AttachParent->AttachChildren.begin(), AttachParent->AttachChildren.end(), this),
                        TEXT("SetupAttachment cannot be used once a component has already had AttachTo used to connect it to a parent."));
                    if (AttachParent == nullptr || AttachParent->AttachChildren.end() == find(AttachParent->AttachChildren.begin(), AttachParent->AttachChildren.end(), this))
                    {
                        SetAttachParent(InParent);
                        /*SetAttachSocketName(InSocketName);
                        SetShouldBeAttached(AttachParent != nullptr);*/
                    }
                }
            }
        }
    }
}

bool USceneComponent::IsAttachedTo(const USceneComponent* TestComp) const
{
    if (TestComp != nullptr)
    {
        for (const USceneComponent* Comp = this->GetAttachParent(); Comp != nullptr; Comp = Comp->GetAttachParent())
        {
            if (TestComp == Comp)
            {
                return true;
            }
        }
    }
    return false;
}

void USceneComponent::SetAttachParent(USceneComponent* NewAttachParent)
{
    AttachParent = NewAttachParent->As<USceneComponent>();
}

void USceneComponent::UpdateComponentToWorldWithParent(USceneComponent* Parent, const FQuat& RelativeRotationQuat)
{
    // 부모가 이전에 업데이트되지 않았다면, 부모 연결 계층을 따라 올라가야 합니다.
    if (Parent && !Parent->bComponentToWorldUpdated)
    {
        //QUICK_SCOPE_CYCLE_COUNTER(STAT_USceneComponent_UpdateComponentToWorldWithParent_Parent);
        Parent->UpdateComponentToWorld();

        // 부모를 업데이트하면(우리가 이미 부모에 부착되어 있는 경우) 업데이트될 수 있으므로, 그냥 반환합니다.
        if (bComponentToWorldUpdated)
        {
            return;
        }
    }

    bComponentToWorldUpdated = true;

    FTransform NewTransform;

    {
        //QUICK_SCOPE_CYCLE_COUNTER(STAT_USceneComponent_UpdateComponentToWorldWithParent_XForm);
        // 새로운 ComponentToWorld 변환을 계산합니다.
        const FTransform RelativeTransform(RelativeRotationQuat, GetRelativeLocation(), GetRelativeScale3D());
        NewTransform = CalcNewComponentToWorld(RelativeTransform, Parent/*, SocketName*/);
    }

    // 변환이 변경되었는지 확인합니다.
    bool bHasChanged;
    {
        //QUICK_SCOPE_CYCLE_COUNTER(STAT_USceneComponent_UpdateComponentToWorldWithParent_HasChanged);
        bHasChanged = !GetComponentTransform().Equals(NewTransform, UE_SMALL_NUMBER);
    }

    // 트랜스폼 변경 여부를 기반으로 여기서 전파합니다. 다른 컴포넌트는 텔레포트 플래그를 사용하여
    // 컴포넌트의 직접 계층 구조 외부의 트랜스폼을 감지할 수 있습니다(예: 액터 트랜스폼).
    if (bHasChanged /*|| Teleport != ETeleportType::None*/)
    {
        //QUICK_SCOPE_CYCLE_COUNTER(STAT_USceneComponent_UpdateComponentToWorldWithParent_Changed);
        // 변환 업데이트
        ComponentToWorld = NewTransform;
        //PropagateTransformUpdate(true, UpdateTransformFlags, Teleport);
    }
    else
    {
        //QUICK_SCOPE_CYCLE_COUNTER(STAT_USceneComponent_UpdateComponentToWorldWithParent_NotChanged);
        //PropagateTransformUpdate(false);
    }

}
