#pragma once
#include "CoreTypes.h"

/**
 * 요소 유형에 독립적인 리소스 배열 인터페이스.
 */
class FResourceArrayInterface
{
    //DECLARE_EXPORTED_TYPE_LAYOUT(FResourceArrayInterface, CORE_API, Abstract);
public:

    virtual ~FResourceArrayInterface() {}

    /**
     * @return 리소스 데이터에 대한 포인터를 반환합니다.
     */
    virtual const void* GetResourceData() const = 0;

    /**
     * @return 리소스 데이터 할당 크기를 반환합니다.
     */
    virtual uint32 GetResourceDataSize() const = 0;

    /** RHI가 리소스 데이터를 복사한 후 CPU의 사본이 더 이상 필요 없는 경우 non-UMA 시스템에서 호출됩니다. */
    virtual void Discard() = 0;

    /**
     * @return 리소스 배열이 정적이고 수정되지 않아야 하는 경우 true를 반환합니다.
     */
    virtual bool IsStatic() const = 0;

    /**
     * @return RHI 리소스가 생성된 후에도 리소스가 그 데이터의 사본을 유지하는 경우 true를 반환합니다.
     */
    virtual bool GetAllowCPUAccess() const = 0;

    /**
     * 리소스 배열이 CPU에 의해 접근될 수 있는지 여부를 설정합니다.
     */
    virtual void SetAllowCPUAccess(bool bInNeedsCPUAccess) = 0;
};


/**
 * 대량 리소스 유형에 대해 직접 GPU 메모리 할당을 허용합니다.
 */
class FResourceBulkDataInterface
{
public:

    virtual ~FResourceBulkDataInterface() {}

    /**
     * @return 미리 할당된 리소스 메모리에 대한 포인터를 반환합니다.
     */
    virtual const void* GetResourceBulkData() const = 0;

    /**
     * @return 리소스 메모리의 크기를 반환합니다.
     */
    virtual uint32 GetResourceBulkDataSize() const = 0;

    /**
     * RHI 리소스를 초기화한 후 메모리를 해제합니다.
     */
    virtual void Discard() = 0;

    enum class EBulkDataType
    {
        Default,
        MediaTexture,
        VREyeBuffer,
    };

    /**
     * @return 특별 처리를 위한 대량 데이터 유형을 반환합니다.
     */
    virtual EBulkDataType GetResourceType() const
    {
        return EBulkDataType::Default;
    }
};
