#pragma once

#include "CoreTypes.h"
#include "Misc/EnumClassFlags.h"

enum class ERHIAccess : uint32
{
    // 리소스의 이전 상태를 알 수 없을 때 사용됩니다.
    // 이는 모든 GPU 캐시를 플러시해야 함을 의미합니다.
    Unknown = 0,

    // 읽기 상태
    CPURead = 1 << 0,
    Present = 1 << 1,
    IndirectArgs = 1 << 2,
    VertexOrIndexBuffer = 1 << 3,
    SRVCompute = 1 << 4,
    SRVGraphics = 1 << 5,
    CopySrc = 1 << 6,
    ResolveSrc = 1 << 7,
    DSVRead = 1 << 8,

    // 읽기-쓰기 상태
    UAVCompute = 1 << 9,
    UAVGraphics = 1 << 10,
    RTV = 1 << 11,
    CopyDest = 1 << 12,
    ResolveDst = 1 << 13,
    DSVWrite = 1 << 14,

    // 레이 트레이싱 가속 구조 상태.
    // AS를 포함하는 버퍼는 항상 이 상태 중 하나여야 합니다.
    // BVHRead -- AS 입력이 빌드/업데이트/복사/추적 명령에 필요한 상태입니다.
    // BVHWrite -- 빌드/업데이트/복사 명령의 AS 출력에 필요한 상태입니다.
    BVHRead = 1 << 15,
    BVHWrite = 1 << 16,

    // 유효하지 않은 해제된 상태 (일시적 리소스)
    Discard = 1 << 17,

    // 셰이딩 속도 소스
    ShadingRateSource = 1 << 18,

    Last = ShadingRateSource,
    None = Unknown,
    Mask = (Last << 1) - 1,

    // 두 가지 가능한 SRV 상태의 마스크
    SRVMask = SRVCompute | SRVGraphics,

    // 두 가지 가능한 UAV 상태의 마스크
    UAVMask = UAVCompute | UAVGraphics,

    // 다른 쓰기 상태와 결합할 수 없는 읽기 전용 상태의 모든 비트를 나타내는 마스크.
    ReadOnlyExclusiveMask = CPURead | Present | IndirectArgs | VertexOrIndexBuffer | SRVGraphics | SRVCompute | CopySrc | ResolveSrc | BVHRead | ShadingRateSource,

    // 다른 쓰기 상태와 결합할 수 없는 컴퓨트 파이프에서의 읽기 전용 상태의 모든 비트를 나타내는 마스크.
    ReadOnlyExclusiveComputeMask = CPURead | IndirectArgs | SRVCompute | CopySrc | BVHRead,

    // 다른 쓰기 상태와 결합될 수 있는 읽기 전용 상태의 모든 비트를 나타내는 마스크.
    ReadOnlyMask = ReadOnlyExclusiveMask | DSVRead | ShadingRateSource,

    // 읽기 가능 상태를 나타내는 모든 비트를 나타내는 마스크로, 쓰기 상태도 포함될 수 있습니다.
    ReadableMask = ReadOnlyMask | UAVMask,

    // 다른 읽기 상태와 결합할 수 없는 쓰기 전용 상태의 모든 비트를 나타내는 마스크.
    WriteOnlyExclusiveMask = RTV | CopyDest | ResolveDst,

    // 다른 읽기 상태와 결합될 수 있는 쓰기 전용 상태의 모든 비트를 나타내는 마스크.
    WriteOnlyMask = WriteOnlyExclusiveMask | DSVWrite,

    // 읽기 상태도 포함될 수 있는 쓰기 가능한 상태의 모든 비트를 나타내는 마스크.
    WritableMask = WriteOnlyMask | UAVMask | BVHWrite
};
ENUM_CLASS_FLAGS(ERHIAccess)


inline constexpr bool IsReadOnlyExclusiveAccess(ERHIAccess Access)
{
	return EnumHasAnyFlags(Access, ERHIAccess::ReadOnlyExclusiveMask) && !EnumHasAnyFlags(Access, ~ERHIAccess::ReadOnlyExclusiveMask);
}

inline constexpr bool IsReadOnlyAccess(ERHIAccess Access)
{
	return EnumHasAnyFlags(Access, ERHIAccess::ReadOnlyMask) && !EnumHasAnyFlags(Access, ~ERHIAccess::ReadOnlyMask);
}

inline constexpr bool IsWriteOnlyAccess(ERHIAccess Access)
{
	return EnumHasAnyFlags(Access, ERHIAccess::WriteOnlyMask) && !EnumHasAnyFlags(Access, ~ERHIAccess::WriteOnlyMask);
}

inline constexpr bool IsWritableAccess(ERHIAccess Access)
{
	return EnumHasAnyFlags(Access, ERHIAccess::WritableMask);
}

inline constexpr bool IsReadableAccess(ERHIAccess Access)
{
	return EnumHasAnyFlags(Access, ERHIAccess::ReadableMask);
}

inline constexpr bool IsInvalidAccess(ERHIAccess Access)
{
	return
		((EnumHasAnyFlags(Access, ERHIAccess::ReadOnlyExclusiveMask) && EnumHasAnyFlags(Access, ERHIAccess::WritableMask)) ||
			(EnumHasAnyFlags(Access, ERHIAccess::WriteOnlyExclusiveMask) && EnumHasAnyFlags(Access, ERHIAccess::ReadableMask)));
}

inline constexpr bool IsValidAccess(ERHIAccess Access)
{
	return !IsInvalidAccess(Access);
}
