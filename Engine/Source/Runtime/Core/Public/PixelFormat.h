#pragma once

#include "CoreTypes.h"
#include "Misc/EnumClassFlags.h"
#undef PF_MAX
enum EPixelFormat : uint8
{
	PF_Unknown = 0,
	PF_A32B32G32R32F = 1,
	PF_B8G8R8A8 = 2,
	PF_G8 = 3, // G8  means Gray/Grey , not Green , typically actually uses a red format with replication of R to RGB
	PF_G16 = 4, // G16 means Gray/Grey like G8
	PF_DXT1 = 5,
	PF_DXT3 = 6,
	PF_DXT5 = 7,
	PF_UYVY = 8,
	PF_FloatRGB = 9,  // FloatRGB == PF_FloatR11G11B10 , NOT 16F usually, but varies
	PF_FloatRGBA = 10, // RGBA16F
	PF_DepthStencil = 11,
	PF_ShadowDepth = 12,
	PF_R32_FLOAT = 13,
	PF_G16R16 = 14,
	PF_G16R16F = 15,
	PF_G16R16F_FILTER = 16,
	PF_G32R32F = 17,
	PF_A2B10G10R10 = 18,
	PF_A16B16G16R16 = 19,
	PF_D24 = 20,
	PF_R16F = 21,
	PF_R16F_FILTER = 22,
	PF_BC5 = 23,
	PF_V8U8 = 24,
	PF_A1 = 25,
	PF_FloatR11G11B10 = 26,
	PF_A8 = 27,
	PF_R32_UINT = 28,
	PF_R32_SINT = 29,
	PF_PVRTC2 = 30,
	PF_PVRTC4 = 31,
	PF_R16_UINT = 32,
	PF_R16_SINT = 33,
	PF_R16G16B16A16_UINT = 34,
	PF_R16G16B16A16_SINT = 35,
	PF_R5G6B5_UNORM = 36,
	PF_R8G8B8A8 = 37,
	PF_A8R8G8B8 = 38,	// Only used for legacy loading; do NOT use!
	PF_BC4 = 39,
	PF_R8G8 = 40,
	PF_ATC_RGB = 41,	// Unsupported Format
	PF_ATC_RGBA_E = 42,	// Unsupported Format
	PF_ATC_RGBA_I = 43,	// Unsupported Format
	PF_X24_G8 = 44,	// Used for creating SRVs to alias a DepthStencil buffer to read Stencil. Don't use for creating textures.
	PF_ETC1 = 45,	// Unsupported Format
	PF_ETC2_RGB = 46,
	PF_ETC2_RGBA = 47,
	PF_R32G32B32A32_UINT = 48,
	PF_R16G16_UINT = 49,
	PF_ASTC_4x4 = 50,	// 8.00 bpp
	PF_ASTC_6x6 = 51,	// 3.56 bpp
	PF_ASTC_8x8 = 52,	// 2.00 bpp
	PF_ASTC_10x10 = 53,	// 1.28 bpp
	PF_ASTC_12x12 = 54,	// 0.89 bpp
	PF_BC6H = 55,
	PF_BC7 = 56,
	PF_R8_UINT = 57,
	PF_L8 = 58,
	PF_XGXR8 = 59,
	PF_R8G8B8A8_UINT = 60,
	PF_R8G8B8A8_SNORM = 61,
	PF_R16G16B16A16_UNORM = 62,
	PF_R16G16B16A16_SNORM = 63,
	PF_PLATFORM_HDR_0 = 64,
	PF_PLATFORM_HDR_1 = 65,	// Reserved.
	PF_PLATFORM_HDR_2 = 66,	// Reserved.
	PF_NV12 = 67,
	PF_R32G32_UINT = 68,
	PF_ETC2_R11_EAC = 69,
	PF_ETC2_RG11_EAC = 70,
	PF_R8 = 71,
	PF_B5G5R5A1_UNORM = 72,
	PF_ASTC_4x4_HDR = 73,
	PF_ASTC_6x6_HDR = 74,
	PF_ASTC_8x8_HDR = 75,
	PF_ASTC_10x10_HDR = 76,
	PF_ASTC_12x12_HDR = 77,
	PF_G16R16_SNORM = 78,
	PF_R8G8_UINT = 79,
	PF_R32G32B32_UINT = 80,
	PF_R32G32B32_SINT = 81,
	PF_R32G32B32F = 82,
	PF_R8_SINT = 83,
	PF_R64_UINT = 84,
	PF_R9G9B9EXP5 = 85,
	PF_P010 = 86,
	PF_ASTC_4x4_NORM_RG = 87, // RG format stored in LA endpoints for better precision (requires RHI support for texture swizzle)
	PF_ASTC_6x6_NORM_RG = 88,
	PF_ASTC_8x8_NORM_RG = 89,
	PF_ASTC_10x10_NORM_RG = 90,
	PF_ASTC_12x12_NORM_RG = 91,
	PF_MAX = 92,
};


enum class EPixelFormatCapabilities : uint32
{
	None = 0,
	Texture1D = 1ull << 1,
	Texture2D = 1ull << 2,
	Texture3D = 1ull << 3,
	TextureCube = 1ull << 4,
	RenderTarget = 1ull << 5,
	DepthStencil = 1ull << 6,
	TextureMipmaps = 1ull << 7,
	TextureLoad = 1ull << 8,
	TextureSample = 1ull << 9,
	TextureGather = 1ull << 10,
	TextureAtomics = 1ull << 11,
	TextureBlendable = 1ull << 12,
	TextureStore = 1ull << 13,

	Buffer = 1ull << 14,
	VertexBuffer = 1ull << 15,
	IndexBuffer = 1ull << 16,
	BufferLoad = 1ull << 17,
	BufferStore = 1ull << 18,
	BufferAtomics = 1ull << 19,

	UAV = 1ull << 20,
	TypedUAVLoad = 1ull << 21,
	TypedUAVStore = 1ull << 22,

	TextureFilterable = 1ull << 23,

	AnyTexture = Texture1D | Texture2D | Texture3D | TextureCube,

	AllTextureFlags = AnyTexture | RenderTarget | DepthStencil | TextureMipmaps | TextureLoad | TextureSample | TextureGather | TextureAtomics | TextureBlendable | TextureStore,
	AllBufferFlags = Buffer | VertexBuffer | IndexBuffer | BufferLoad | BufferStore | BufferAtomics,
	AllUAVFlags = UAV | TypedUAVLoad | TypedUAVStore,

	AllFlags = AllTextureFlags | AllBufferFlags | AllUAVFlags
};
ENUM_CLASS_FLAGS(EPixelFormatCapabilities);

/**
 * 픽셀 포맷에 대한 정보입니다. 이 구조체의 대부분은 정적 초기화 이후에 유효하지만, RHI 모듈에서 초기화된 상태를 유지하는 일부 상태가 있습니다. 이러한 상태는 RHI가 없는 일반 프로그램에서는 사용되지 않아야 합니다 (댓글에서 언급됨).
 */
struct FPixelFormatInfo
{
	FPixelFormatInfo() = delete;
	FPixelFormatInfo(
		EPixelFormat InUnrealFormat,
		const TCHAR* InName,
		int32 InBlockSizeX,
		int32 InBlockSizeY,
		int32 InBlockSizeZ,
		int32 InBlockBytes,
		int32 InNumComponents,
		bool  InSupported);

	const TCHAR* Name;
	EPixelFormat                UnrealFormat;
	int32                       BlockSizeX;
	int32                       BlockSizeY;
	int32                       BlockSizeZ;
	int32                       BlockBytes;
	int32                       NumComponents;

	/** 플랫폼별 포맷에 대한 기능 (RHI 모듈에 의해 초기화됨 - 그렇지 않으면 유효하지 않음) */
	EPixelFormatCapabilities    Capabilities = EPixelFormatCapabilities::None;

	/** 플랫폼별로 변환된 포맷 (RHI 모듈에 의해 초기화됨 - 그렇지 않으면 유효하지 않음) */
	uint32                      PlatformFormat{ 0 };

	/** 현재 플랫폼/렌더링 조합에서 텍스처 포맷이 지원되는지 여부 */
	uint8                       Supported : 1;

	// false인 경우, 32비트 플로트로 간주됩니다 (RHI 모듈에 의해 초기화됨 - 그렇지 않으면 유효하지 않음)
	uint8                       bIs24BitUnormDepthStencil : 1;

	/**
	 * 2D/3D 이미지/텍스처 크기를 바이트 단위로 가져옵니다. 이는 인코딩된 이미지 데이터의 저장을 위한 것이며, GPU 정렬/패딩 제약 조건을 조정하지 않습니다. 또한 타일 또는 패킹된 mip 테일 (즉, 콘솔용으로 요리된 mip)에는 유효하지 않습니다. 일반적인 블록 기반 픽셀 포맷의 텍스처/이미지 작업 시에만 사용하십시오.
	 */
	CORE_API uint64 Get2DImageSizeInBytes(uint32 InWidth, uint32 InHeight) const;
	CORE_API uint64 Get2DTextureMipSizeInBytes(uint32 InTextureWidth, uint32 InTextureHeight, uint32 InMipIndex) const;
	CORE_API uint64 Get2DTextureSizeInBytes(uint32 InTextureWidth, uint32 InTextureHeight, uint32 InMipCount) const;
	CORE_API uint64 Get3DImageSizeInBytes(uint32 InWidth, uint32 InHeight, uint32 InDepth) const;
	CORE_API uint64 Get3DTextureMipSizeInBytes(uint32 InTextureWidth, uint32 InTextureHeight, uint32 InTextureDepth, uint32 InMipIndex) const;
	CORE_API uint64 Get3DTextureSizeInBytes(uint32 InTextureWidth, uint32 InTextureHeight, uint32 InTextureDepth, uint32 InMipCount) const;

	/**
	 * 주어진 크기를 수용하는 데 필요한 압축 블록의 수를 가져옵니다.
	 */
	CORE_API uint64 GetBlockCountForWidth(uint32 InWidth) const;
	CORE_API uint64 GetBlockCountForHeight(uint32 InHeight) const;
};


extern CORE_API FPixelFormatInfo GPixelFormats[PF_MAX];		// EPixelFormat의 멤버를 FPixelFormatInfo로 매핑하여 포맷을 설명합니다.