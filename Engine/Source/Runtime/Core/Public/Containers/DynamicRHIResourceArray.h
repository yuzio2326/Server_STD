#pragma once
#include "CoreTypes.h"
#include "ResourceArray.h"
#include "Logging/Logger.h"

/**
 * UMA 렌더링 리소스에 사용할 수 있는 메모리를 할당하는 배열입니다.
 * 동적으로 바인딩된 RHI에서는 기본 배열 유형과 다르지 않습니다.
 * 동적으로 바인딩된 RHI 구현 중 어떤 것도 UMA를 가지고 있지 않기 때문입니다.
 *
 * @param Alignment - 할당에 사용할 메모리 정렬
 */
template< typename ElementType/*, uint32 Alignment = DEFAULT_ALIGNMENT*/>
class TResourceArray
    : public FResourceArrayInterface
    , public TArray<ElementType/*, TMemoryImageAllocator<Alignment>*/>
{
	using ParentArrayType = TArray<ElementType>;//, TMemoryImageAllocator<Alignment >> ;
	//DECLARE_INLINE_TYPE_LAYOUT_EXPLICIT_BASES(TResourceArray, Virtual, FResourceArrayInterface, ParentArrayType);
public:
	using Super = ParentArrayType;

    /**
* 생성자
*/
    TResourceArray(bool InNeedsCPUAccess = false)
        : Super()
        , bNeedsCPUAccess(InNeedsCPUAccess)
    {
    }

    TResourceArray(TResourceArray&&) = default;
    TResourceArray(const TResourceArray&) = default;
    TResourceArray& operator=(TResourceArray&&) = default;
    TResourceArray& operator=(const TResourceArray&) = default;

    virtual ~TResourceArray() = default;


    // FResourceArrayInterface

    /**
    * @return 리소스 데이터에 대한 포인터를 반환합니다.
    */
    virtual const void* GetResourceData() const
    {
        return &(*this)[0];
    }

    /**
    * @return 리소스 데이터 할당 크기를 반환합니다.
    */
    virtual uint32 GetResourceDataSize() const
    {
        if (this->size() > UINT32_MAX / sizeof(ElementType))
        {
            E_LOG(Fatal, TEXT("리소스 데이터 크기가 uint32에 대해 너무 큽니다. 오버플로우가 발생합니다. 더 큰 데이터 타입으로 계산하거나 요소 수를 줄이세요. sizeof(ElementType): {}"), sizeof(ElementType));
            return 0;
        }
        return this->size() * sizeof(ElementType);
    }

    virtual uint32 GetTypeSize() const
    {
        return sizeof(ElementType);
    }

    /**
    * RHI가 리소스 데이터를 복사한 후 CPU의 사본이 더 이상 필요 없는 경우 non-UMA 시스템에서 호출됩니다.
    * 클라이언트에서만 리소스 메모리를 폐기하며, CPU가 이를 접근할 필요가 없는 경우에만 해당합니다.
    * 비클라이언트는 데이터를 직렬화할 필요가 있을 수 있으므로 데이터를 폐기할 수 없습니다.
    */
    virtual void Discard()
    {
        if (!bNeedsCPUAccess /*&& FPlatformProperties::RequiresCookedData() && !IsRunningCommandlet()*/)
        {
            this->clear();
        }
    }

    /**
    * @return 리소스 배열이 정적이고 수정되지 않아야 하는 경우 true를 반환합니다.
    */
    virtual bool IsStatic() const
    {
        return false;
    }

    /**
    * @return RHI 리소스가 생성된 후에도 리소스가 그 데이터의 사본을 유지하는 경우 true를 반환합니다.
    */
    virtual bool GetAllowCPUAccess() const
    {
        return bNeedsCPUAccess;
    }

    /**
    * 리소스 배열이 CPU에 의해 접근될 수 있는지 여부를 설정합니다.
    */
    virtual void SetAllowCPUAccess(bool bInNeedsCPUAccess)
    {
        bNeedsCPUAccess = bInNeedsCPUAccess;
    }

    // 할당 연산자.
    TResourceArray& operator=(const Super& Other)
    {
        Super::operator=(Other);
        return *this;
    }

    ///**
    //* 데이터를 단일 블록으로 직렬화합니다. 자세한 내용은 TArray::BulkSerialize를 참조하십시오.
    //*
    //* 중요:
    //*   - 이는 UnrealTemplate.h의 TArray::BulkSerialize에서 오버라이드된 것입니다. 변경 사항을 적절히 반영해 주세요.
    //*
    //* @param Ar 이 TArray를 직렬화할 FArchive
    //*/
    //void BulkSerialize(FArchive& Ar, bool bForcePerElementSerialization = false)
    //{
    //    Super::BulkSerialize(Ar, bForcePerElementSerialization);
    //}

private:
	/**
	* 이 배열에 CPU 접근이 필요한 경우 true입니다.
	* CPU 접근이 필요하지 않으면 RHI 리소스가 생성된 후 리소스가 해제됩니다.
	*/
	//LAYOUT_FIELD(bool, bNeedsCPUAccess);
	bool bNeedsCPUAccess;
};