#pragma once
#include "RHIDefinitions.h"

/** 선호하는 벤더의 VendorID를 반환하며, 지정되지 않은 경우 -1을 반환합니다. */
extern RHI_API EGpuVendorId RHIGetPreferredAdapterVendor();

/**
 * 주어진 원시 유형의 프리미티브 수에 대해 버텍스 수를 계산합니다.
 * @param NumPrimitives 프리미티브의 수.
 * @param PrimitiveType 프리미티브의 유형.
 * @returns 버텍스의 수.
 */
inline uint32 GetVertexCountForPrimitiveCount(uint32 NumPrimitives, uint32 PrimitiveType)
{
    static_assert(PT_Num == 6, "이 함수는 업데이트가 필요합니다");
    uint32 Factor = (PrimitiveType == PT_TriangleList) ? 3 : (PrimitiveType == PT_LineList) ? 2 : (PrimitiveType == PT_RectList) ? 3 : 1;
    uint32 Offset = (PrimitiveType == PT_TriangleStrip) ? 2 : 0;

    return NumPrimitives * Factor + Offset;
}
