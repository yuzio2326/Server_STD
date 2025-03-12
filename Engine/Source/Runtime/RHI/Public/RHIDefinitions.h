#pragma once
#include "CoreMinimal.h"

/** 셰이더 매개변수 구조체의 정렬은 16바이트 경계여야 합니다. */
#define SHADER_PARAMETER_STRUCT_ALIGNMENT 16

enum class EGpuVendorId : uint32
{
	Unknown = 0xffffffff,
	NotQueried = 0,

	Amd = 0x1002,
	ImgTec = 0x1010,
	Nvidia = 0x10DE,
	Arm = 0x13B5,
	Broadcom = 0x14E4,
	Qualcomm = 0x5143,
	Intel = 0x8086,
	Apple = 0x106B,
	Vivante = 0x7a05,
	VeriSilicon = 0x1EB1,
	SamsungAMD = 0x144D,
	Microsoft = 0x1414,

	Kazan = 0x10003,	// VkVendorId
	Codeplay = 0x10004,	// VkVendorId
	Mesa = 0x10005,	// VkVendorId
};

inline EGpuVendorId RHIConvertToGpuVendorId(uint32 VendorId)
{
	switch ((EGpuVendorId)VendorId)
	{
	case EGpuVendorId::NotQueried:
		return EGpuVendorId::NotQueried;

	case EGpuVendorId::Amd:
	case EGpuVendorId::Mesa:
	case EGpuVendorId::ImgTec:
	case EGpuVendorId::Nvidia:
	case EGpuVendorId::Arm:
	case EGpuVendorId::Broadcom:
	case EGpuVendorId::Qualcomm:
	case EGpuVendorId::Intel:
	case EGpuVendorId::SamsungAMD:
	case EGpuVendorId::Microsoft:
		return (EGpuVendorId)VendorId;

	default:
		break;
	}

	return EGpuVendorId::Unknown;
}


/** An enumeration of the different RHI reference types. */
enum ERHIResourceType : uint8
{
	RRT_None,

	RRT_SamplerState,
	RRT_RasterizerState,
	RRT_DepthStencilState,
	RRT_BlendState,
	RRT_VertexDeclaration,
	RRT_VertexShader,
	RRT_MeshShader,
	RRT_AmplificationShader,
	RRT_PixelShader,
	RRT_GeometryShader,
	RRT_RayTracingShader,
	RRT_ComputeShader,
	RRT_GraphicsPipelineState,
	RRT_ComputePipelineState,
	RRT_RayTracingPipelineState,
	RRT_BoundShaderState,
	RRT_UniformBufferLayout,
	RRT_UniformBuffer,
	RRT_Buffer,
	RRT_Texture,
	// @todo: texture type unification - remove these
	RRT_Texture2D,
	RRT_Texture2DArray,
	RRT_Texture3D,
	RRT_TextureCube,
	// @todo: texture type unification - remove these
	RRT_TextureReference,
	RRT_TimestampCalibrationQuery,
	RRT_GPUFence,
	RRT_RenderQuery,
	RRT_RenderQueryPool,
	RRT_Viewport,
	RRT_UnorderedAccessView,
	RRT_ShaderResourceView,
	RRT_RayTracingAccelerationStructure,
	RRT_StagingBuffer,
	RRT_CustomPresent,
	RRT_ShaderLibrary,
	RRT_PipelineBinaryLibrary,
	RRT_ShaderBundle,

	RRT_Num
};

/** 텍스처의 차원을 설명합니다. */
enum class ETextureDimension : uint8
{
	Texture2D,
	Texture2DArray,
	Texture3D,
	TextureCube,
	TextureCubeArray
};

/** 텍스처 생성에 사용되는 플래그 */
enum class ETextureCreateFlags : uint64
{
	None = 0,

	// 텍스처를 렌더 타겟으로 사용할 수 있습니다.
	RenderTargetable = 1ull << 0,
	// 텍스처를 리졸브 타겟으로 사용할 수 있습니다.
	ResolveTargetable = 1ull << 1,
	// 텍스처를 깊이-스텐실 타겟으로 사용할 수 있습니다.
	DepthStencilTargetable = 1ull << 2,
	// 텍스처를 셰이더 리소스로 사용할 수 있습니다.
	ShaderResource = 1ull << 3,
	// 텍스처가 sRGB 감마 공간에서 인코딩됩니다.
	SRGB = 1ull << 4,
	// 텍스처 데이터를 CPU가 쓸 수 있습니다.
	CPUWritable = 1ull << 5,
	// 텍스처가 타일링되지 않은 형식으로 생성됩니다.
	NoTiling = 1ull << 6,
	// 텍스처가 비디오 디코딩에 사용됩니다.
	VideoDecode = 1ull << 7,
	// 텍스처는 매 프레임 업데이트될 수 있습니다.
	Dynamic = 1ull << 8,
	// 텍스처가 읽기 가능한 렌더 패스 첨부 파일로 사용됩니다.
	InputAttachmentRead = 1ull << 9,
	// 텍스처가 포베이션 첨부 파일을 나타냅니다.
	Foveation = 1ull << 10,
	// 가능하면 3D 내부 표면 타일링 모드를 선호합니다.
	Tiling3D = 1ull << 11,
	// 이 텍스처는 GPU나 CPU 백업이 없습니다. 모바일과 같은 타일 기반 딜레이드 렌더링(TBDR) GPU의 타일 메모리에만 존재합니다.
	Memoryless = 1ull << 12,
	// 나중에 mip 생성이 가능한 플래그로 텍스처를 생성합니다. D3D11에만 적용됩니다.
	GenerateMipCapable = 1ull << 13,
	// 텍스처가 FastVRAM에 부분적으로 할당될 수 있습니다.
	FastVRAMPartialAlloc = 1ull << 14,
	// 관련된 셰이더 리소스 보기를 생성하지 않습니다. D3D11 및 D3D12에만 적용됩니다.
	DisableSRVCreation = 1ull << 15,
	// 이 텍스처에 Delta Color Compression (DCC)을 사용할 수 없습니다.
	DisableDCC = 1ull << 16,
	// UnorderedAccessView (DX11 전용)
	// 경고: 이 플래그로 할당된 렌더 타겟을 사용하는 드로우 호출 간에 추가 동기화가 발생합니다. 신중하게 사용하십시오.
	UAV = 1ull << 17,
	// 화면에 표시될 렌더 타겟 텍스처 (백 버퍼)
	Presentable = 1ull << 18,
	// 텍스처 데이터는 CPU에서 접근 가능합니다.
	CPUReadback = 1ull << 19,
	// 텍스처가 오프라인으로 처리되었습니다 (현재 플랫폼에 대한 텍스처 변환 프로세스를 통해).
	OfflineProcessed = 1ull << 20,
	// 사용할 수 있는 경우 텍스처는 Fast VRAM에 배치해야 합니다 (힌트 전용).
	FastVRAM = 1ull << 21,
	// 기본적으로 텍스처가 목록에 표시되지 않음 - 혼란을 줄이기 위해. 전체 옵션을 사용할 때 무시할 수 있습니다.
	HideInVisualizeTexture = 1ull << 22,
	// 텍스처는 물리적 메모리 할당 없이 가상 메모리에서 생성되어야 합니다.
	// RHIVirtualTextureSetFirstMipInMemory를 호출하여 물리적 메모리를 할당하고, 
	// RHIVirtualTextureSetFirstMipVisible을 호출하여 GPU에 처음으로 볼 수 있는 mip을 맵핑해야 합니다.
	Virtual = 1ull << 23,
	// 텍스처의 각 배열 슬라이스에 대해 RenderTargetView를 생성합니다.
	// 경고: 리소스가 생성될 때 이 옵션이 지정된 경우, 다른 슬라이스로 라우팅하기 위해 SV_RenderTargetArrayIndex를 사용할 수 없습니다!
	TargetArraySlicesIndependently = 1ull << 24,
	// DX9 또는 다른 장치와 공유할 수 있는 텍스처
	Shared = 1ull << 25,
	// RenderTarget은 전체 텍스처의 빠른 지우기 기능을 사용하지 않습니다.
	NoFastClear = 1ull << 26,
	// 텍스처는 깊이 스텐실 리졸브 타겟입니다.
	DepthStencilResolveTarget = 1ull << 27,
	// 스트리밍 풀 예산에 포함되어야 하는 스트리밍 가능한 2D 텍스처를 나타냅니다.
	Streamable = 1ull << 28,
	// 렌더 타겟은 빠른 지우기를 완료하지 않습니다. 캐시 및 메타 데이터를 플러시하지만 지우기는 건너뜁니다 (메타 데이터를 망칠 가능성을 피함).
	NoFastClearFinalize = 1ull << 29,
	// 텍스처는 원자적 작업을 지원해야 합니다.
	Atomic64Compatible = 1ull << 30,
	// 타일링 모드로 인해 128^3 볼륨 텍스처가 4배로 부풀어 오르는 경우를 해결합니다.
	ReduceMemoryWithTilingMode = 1ull << 31,
	// 텍스처는 원자적 작업을 지원해야 합니다.
	AtomicCompatible = 1ull << 33,
	// 텍스처는 외부 접근을 위해 할당되어야 합니다. Vulkan 전용
	External = 1ull << 34,
	// 다중 GPU 시나리오에서 GPU 간 자동 전송을 허용하지 않습니다. 예를 들어, 수동으로 전송하는 경우.
	MultiGPUGraphIgnore = 1ull << 35,
	// 실험적: 텍스처를 내부적으로 물리적 메모리 백업 없이 예약된(예: 타일링/희소/가상) 리소스로 생성할 수 있도록 합니다.
	ReservedResource = 1ull << 37,
	// 실험적: ReservedResource 플래그와 함께 사용하여 생성 시 메모리를 즉시 할당하고 커밋합니다. 단일 큰 할당 대신 여러 작은 물리적 메모리 할당을 사용할 수 있습니다.
	ImmediateCommit = 1ull << 38,

	// 총 텍스처 할당 크기를 추적할 때 스트리밍 메모리에 이 텍스처를 포함하지 마십시오.
	ForceIntoNonStreamingMemoryTracking = 1ull << 39,

	// 이 플래그가 표시된 텍스처는 생성 후 즉시 회수하여 GPU를 페이지 폴트로 고의적으로 충돌시키기 위한 것입니다.
	Invalid = 1ull << 40,
};
ENUM_CLASS_FLAGS(ETextureCreateFlags);

// Compatibility defines
#define TexCreate_None                           ETextureCreateFlags::None
#define TexCreate_RenderTargetable               ETextureCreateFlags::RenderTargetable
#define TexCreate_ResolveTargetable              ETextureCreateFlags::ResolveTargetable
#define TexCreate_DepthStencilTargetable         ETextureCreateFlags::DepthStencilTargetable
#define TexCreate_ShaderResource                 ETextureCreateFlags::ShaderResource
#define TexCreate_SRGB                           ETextureCreateFlags::SRGB
#define TexCreate_CPUWritable                    ETextureCreateFlags::CPUWritable
#define TexCreate_NoTiling                       ETextureCreateFlags::NoTiling
#define TexCreate_VideoDecode                    ETextureCreateFlags::VideoDecode
#define TexCreate_Dynamic                        ETextureCreateFlags::Dynamic
#define TexCreate_InputAttachmentRead            ETextureCreateFlags::InputAttachmentRead
#define TexCreate_Foveation                      ETextureCreateFlags::Foveation
#define TexCreate_3DTiling                       ETextureCreateFlags::Tiling3D
#define TexCreate_Memoryless                     ETextureCreateFlags::Memoryless
#define TexCreate_GenerateMipCapable             ETextureCreateFlags::GenerateMipCapable
#define TexCreate_FastVRAMPartialAlloc           ETextureCreateFlags::FastVRAMPartialAlloc
#define TexCreate_DisableSRVCreation             ETextureCreateFlags::DisableSRVCreation
#define TexCreate_DisableDCC                     ETextureCreateFlags::DisableDCC
#define TexCreate_UAV                            ETextureCreateFlags::UAV
#define TexCreate_Presentable                    ETextureCreateFlags::Presentable
#define TexCreate_CPUReadback                    ETextureCreateFlags::CPUReadback
#define TexCreate_OfflineProcessed               ETextureCreateFlags::OfflineProcessed
#define TexCreate_FastVRAM                       ETextureCreateFlags::FastVRAM
#define TexCreate_HideInVisualizeTexture         ETextureCreateFlags::HideInVisualizeTexture
#define TexCreate_Virtual                        ETextureCreateFlags::Virtual
#define TexCreate_TargetArraySlicesIndependently ETextureCreateFlags::TargetArraySlicesIndependently
#define TexCreate_Shared                         ETextureCreateFlags::Shared
#define TexCreate_NoFastClear                    ETextureCreateFlags::NoFastClear
#define TexCreate_DepthStencilResolveTarget      ETextureCreateFlags::DepthStencilResolveTarget
#define TexCreate_Streamable                     ETextureCreateFlags::Streamable
#define TexCreate_NoFastClearFinalize            ETextureCreateFlags::NoFastClearFinalize
#define TexCreate_ReduceMemoryWithTilingMode     ETextureCreateFlags::ReduceMemoryWithTilingMode
#define TexCreate_Transient                      ETextureCreateFlags::Transient
#define TexCreate_AtomicCompatible               ETextureCreateFlags::AtomicCompatible
#define TexCreate_External                       ETextureCreateFlags::External
#define TexCreate_MultiGPUGraphIgnore            ETextureCreateFlags::MultiGPUGraphIgnore
#define TexCreate_ReservedResource               ETextureCreateFlags::ReservedResource
#define TexCreate_ImmediateCommit                ETextureCreateFlags::ImmediateCommit
#define TexCreate_Invalid                        ETextureCreateFlags::Invalid

enum class ERHIZBuffer
{
	// 이 값을 변경하기 전에 모든 수학적 및 셰이더 가정이 올바른지 확인하십시오! 또한 C++ 가정을 static_assert(ERHIZBuffer::IsInvertedZBuffer(), ...);로 래핑하십시오.
	// 셰이더에서는 Definitions.usf의 HAS_INVERTED_Z_BUFFER를 업데이트해야 합니다.
	FarPlane = 0,
	NearPlane = 1,

	// API가 Inverted Z 버퍼를 사용하는지 여부를 알기 위한 'bool' 값
	IsInverted = (int32)((int32)ERHIZBuffer::FarPlane < (int32)ERHIZBuffer::NearPlane),
};

/**
 * 렌더 타겟이 설정될 때 수행할 작업.
 */
enum class ERenderTargetLoadAction : uint8
{
	// 렌더 타겟의 변경되지 않은 내용은 정의되지 않습니다. 기존 내용은 보존되지 않습니다.
	ENoAction,

	// 기존 내용이 보존됩니다.
	ELoad,

	// 렌더 타겟이 리소스에 지정된 빠른 클리어 값으로 지워집니다.
	EClear,

	Num,
	NumBits = 2,
};
static_assert((uint32)ERenderTargetLoadAction::Num <= (1 << (uint32)ERenderTargetLoadAction::NumBits), "ERenderTargetLoadAction::Num는 ERenderTargetLoadAction::NumBits에 맞지 않습니다");

/**
 * 렌더 타겟이 해제되거나 패스가 끝날 때 수행할 작업.
 */
enum class ERenderTargetStoreAction : uint8
{
	// 패스 동안 생성된 렌더 타겟의 내용이 메모리에 저장되지 않습니다.
	ENoAction,

	// 패스 동안 생성된 렌더 타겟의 내용이 메모리에 저장됩니다.
	EStore,

	// 패스 동안 생성된 렌더 타겟의 내용이 박스 필터를 사용하여 해결된 후 메모리에 저장됩니다.
	EMultisampleResolve,

	Num,
	NumBits = 2,
};
static_assert((uint32)ERenderTargetStoreAction::Num <= (1 << (uint32)ERenderTargetStoreAction::NumBits), "ERenderTargetStoreAction::Num는 ERenderTargetStoreAction::NumBits에 맞지 않습니다");

/** 동시에 쓰기 가능한 렌더 타겟의 수. */
enum
{
	MaxSimultaneousRenderTargets = 8,
	MaxSimultaneousRenderTargets_NumBits = 3,
};
static_assert(MaxSimultaneousRenderTargets <= (1 << MaxSimultaneousRenderTargets_NumBits), "MaxSimultaneousRenderTargets는 MaxSimultaneousRenderTargets_NumBits에 맞지 않습니다");


enum EShaderFrequency : uint8
{
	SF_Vertex = 0,
	SF_Mesh = 1,
	SF_Amplification = 2,
	SF_Pixel = 3,
	SF_Geometry = 4,
	SF_Compute = 5,
	SF_RayGen = 6,
	SF_RayMiss = 7,
	SF_RayHitGroup = 8,
	SF_RayCallable = 9,

	SF_NumFrequencies = 10,

	// Number of standard shader frequencies for graphics pipeline (excluding compute)
	SF_NumGraphicsFrequencies = 5,

	// Number of standard shader frequencies (including compute)
	SF_NumStandardFrequencies = 6,

	SF_NumBits = 4,
};
static_assert(SF_NumFrequencies <= (1 << SF_NumBits), "SF_NumFrequencies will not fit on SF_NumBits");

inline bool IsValidGraphicsFrequency(EShaderFrequency InShaderFrequency)
{
	switch (InShaderFrequency)
	{
	case SF_Vertex:        return true;
#if PLATFORM_SUPPORTS_MESH_SHADERS
	case SF_Mesh:          return true;
	case SF_Amplification: return true;
#endif
	case SF_Pixel:         return true;
	case SF_Geometry:      return true;
	}
	return false;
}

enum EVertexElementType : uint8
{
	VET_None,
	VET_Float1,
	VET_Float2,
	VET_Float3,
	VET_Float4,
	VET_PackedNormal,  // FPackedNormal
	VET_UByte4,
	VET_UByte4N,
	VET_Color,
	VET_Short2,
	VET_Short4,
	VET_Short2N,       // 16비트 워드, (value/32767.0,value/32767.0,0,0,1)로 정규화
	VET_Half2,         // 1비트 부호, 5비트 지수, 10비트 가수의 16비트 부동 소수점
	VET_Half4,
	VET_Short4N,       // 4개의 16비트 워드, 정규화됨
	VET_UShort2,
	VET_UShort4,
	VET_UShort2N,      // 16비트 워드, (value/65535.0,value/65535.0,0,0,1)로 정규화
	VET_UShort4N,      // 4개의 16비트 워드, 부호 없는 정규화
	VET_URGB10A2N,     // 10비트 r, g, b, 2비트 a, (value/1023.0f, value/1023.0f, value/1023.0f, value/3.0f)로 정규화
	VET_UInt,
	VET_MAX,

	VET_NumBits = 5,
};
static_assert(VET_MAX <= (1 << VET_NumBits), "VET_MAX는 VET_NumBits에 맞지 않습니다");

/**
 * 버텍스 선언에서 사용할 수 있는 최대 버텍스 요소 수.
 */
enum
{
	MaxVertexElementCount = 17,
	MaxVertexElementCount_NumBits = 5,
};
static_assert(MaxVertexElementCount <= (1 << MaxVertexElementCount_NumBits), "MaxVertexElementCount는 MaxVertexElementCount_NumBits에 맞지 않습니다");

/**
 * 버텍스 및 인덱스 버퍼를 위한 리소스 사용 플래그.
 */
enum class EBufferUsageFlags : uint32
{
	None = 0,

	/** 버퍼는 한 번만 작성됩니다. */
	Static = 1 << 0,

	/** 버퍼는 가끔 작성되며, GPU는 읽기 전용이고 CPU는 쓰기 전용입니다. 데이터의 수명은 다음 업데이트 때까지 또는 버퍼가 파괴될 때까지입니다. */
	Dynamic = 1 << 1,

	/** 버퍼의 데이터 수명은 한 프레임입니다. 각 프레임마다 작성되어야 하며, 그렇지 않으면 매 프레임마다 새로 만들어져야 합니다. */
	Volatile = 1 << 2,

	/** 버퍼에 대한 비순차적 접근 뷰를 생성할 수 있도록 합니다. */
	UnorderedAccess = 1 << 3,

	/** 바이트 주소 버퍼를 생성합니다. 이는 기본적으로 uint32 타입을 사용하는 구조화된 버퍼입니다. */
	ByteAddressBuffer = 1 << 4,

	/** GPU가 복사의 소스로 사용할 버퍼입니다. */
	SourceCopy = 1 << 5,

	/** 스트림 출력 대상으로 바인딩될 수 있는 버퍼를 생성합니다. */
	//StreamOutput            UE_DEPRECATED(5.3, "StreamOut은 지원되지 않습니다") = 1 << 6,

	/** DispatchIndirect 또는 DrawIndirect에서 사용하는 인자를 포함하는 버퍼를 생성합니다. */
	DrawIndirect = 1 << 7,

	/**
	 * 셰이더 리소스로 바인딩될 수 있는 버퍼를 생성합니다.
	 * 이는 보통 셰이더 리소스로 사용되지 않는 버퍼 타입(예: 버텍스 버퍼)에만 필요합니다.
	 */
	ShaderResource = 1 << 8,

	/** 이 버퍼가 직접 CPU에서 접근할 수 있도록 요청합니다. */
	KeepCPUAccessible = 1 << 9,

	/** 버퍼는 빠른 VRAM에 있어야 합니다(힌트만 제공). BUF_Transient가 필요합니다. */
	FastVRAM = 1 << 10,

	/** 외부 RHI 또는 프로세스와 공유할 수 있는 버퍼를 생성합니다. */
	Shared = 1 << 12,

	/**
	 * 버퍼는 불투명한 레이 트레이싱 가속 구조 데이터를 포함합니다.
	 * 이 플래그가 설정된 리소스는 셰이더 단계에 직접 바인딩될 수 없으며, 레이 트레이싱 API에서만 사용할 수 있습니다.
	 * 이 플래그는 Static 및 ReservedResource를 제외한 모든 다른 버퍼 플래그와 상호 배타적입니다.
	*/
	AccelerationStructure = 1 << 13,

	VertexBuffer = 1 << 14,
	IndexBuffer = 1 << 15,
	StructuredBuffer = 1 << 16,

	/** 버퍼 메모리는 드라이버 별칭을 통해 공유되지 않고 여러 GPU에 독립적으로 할당됩니다. */
	MultiGPUAllocate = 1 << 17,

	/**
	 * 멀티-GPU 시나리오에서 렌더 그래프가 GPU 간 전송을 신경 쓰지 않도록 합니다.
	 * 스트리밍 요청 버퍼와 같이 CPU로 읽어들이거나, 각 프레임마다 CPU에서 작성된 경우 등 다른 GPU가 데이터를 실제로 신경 쓰지 않는 경우에 유용합니다.
	*/
	MultiGPUGraphIgnore = 1 << 18,

	/** 레이 트레이싱 가속 구조를 구축하기 위한 스크래치 버퍼로 사용할 수 있는 버퍼입니다.
	 * 이는 비순차적 접근을 암시합니다. 버퍼 정렬만 변경하며 다른 플래그와 결합할 수 있습니다.
	**/
	RayTracingScratch = (1 << 19) | UnorderedAccess,

	/** 버퍼는 스트리밍을 위한 자리표시자이며, 기본 GPU 리소스를 포함하지 않습니다. */
	NullResource = 1 << 20,

	/** 플랫폼이 Uniform Buffer Objects를 지원하는 경우 유니폼 버퍼로 사용할 수 있는 버퍼입니다. */
	UniformBuffer = 1 << 21,

	/**
	* 실험적: 버퍼를 물리적 메모리 백업 없이 내부적으로 예약된(타일링/스파스/가상) 리소스로 생성할 수 있습니다.
	* 동적 및 다른 플래그와 함께 사용될 수 없습니다.
	*/
	ReservedResource = 1 << 22,

	// 도우미 비트 마스크
	AnyDynamic = (Dynamic | Volatile),
};
ENUM_CLASS_FLAGS(EBufferUsageFlags);

#define BUF_None                   EBufferUsageFlags::None
#define BUF_Static                 EBufferUsageFlags::Static
#define BUF_Dynamic                EBufferUsageFlags::Dynamic
#define BUF_Volatile               EBufferUsageFlags::Volatile
#define BUF_UnorderedAccess        EBufferUsageFlags::UnorderedAccess
#define BUF_ByteAddressBuffer      EBufferUsageFlags::ByteAddressBuffer
#define BUF_SourceCopy             EBufferUsageFlags::SourceCopy
//#define BUF_StreamOutput           EBufferUsageFlags::StreamOutput
#define BUF_DrawIndirect           EBufferUsageFlags::DrawIndirect
#define BUF_ShaderResource         EBufferUsageFlags::ShaderResource
#define BUF_KeepCPUAccessible      EBufferUsageFlags::KeepCPUAccessible
#define BUF_FastVRAM               EBufferUsageFlags::FastVRAM
//#define BUF_Transient              EBufferUsageFlags::Transient
#define BUF_Shared                 EBufferUsageFlags::Shared
#define BUF_AccelerationStructure  EBufferUsageFlags::AccelerationStructure
#define BUF_RayTracingScratch      EBufferUsageFlags::RayTracingScratch
#define BUF_VertexBuffer           EBufferUsageFlags::VertexBuffer
#define BUF_IndexBuffer            EBufferUsageFlags::IndexBuffer
#define BUF_StructuredBuffer       EBufferUsageFlags::StructuredBuffer
#define BUF_AnyDynamic             EBufferUsageFlags::AnyDynamic
#define BUF_MultiGPUAllocate       EBufferUsageFlags::MultiGPUAllocate
#define BUF_MultiGPUGraphIgnore    EBufferUsageFlags::MultiGPUGraphIgnore
#define BUF_NullResource           EBufferUsageFlags::NullResource
#define BUF_UniformBuffer          EBufferUsageFlags::UniformBuffer
#define BUF_ReservedResource       EBufferUsageFlags::ReservedResource

enum EPrimitiveType
{
	// 삼각형 N을 3개의 버텍스 끝점(3*N+0, 3*N+1, 3*N+2)으로 정의하는 토폴로지입니다.
	PT_TriangleList,

	// 삼각형 N을 3개의 버텍스 끝점(N+0, N+1, N+2)으로 정의하는 토폴로지입니다.
	PT_TriangleStrip,

	// 라인을 2개의 버텍스 끝점(2*N+0, 2*N+1)으로 정의하는 토폴로지입니다.
	PT_LineList,

	// 사각형 N을 4개의 버텍스 끝점(4*N+0, 4*N+1, 4*N+2, 4*N+3)으로 정의하는 토폴로지입니다.
	// GRHISupportsQuadTopology가 true인 경우에만 지원됩니다.
	PT_QuadList,

	// 포인트 N을 단일 버텍스(N)으로 정의하는 토폴로지입니다.
	PT_PointList,

	// 화면 정렬 직사각형 N을 3개의 버텍스 모서리(3*N + 0은 좌상단 모서리, 3*N + 1은 우상단 모서리, 3*N + 2는 좌하단 모서리)로 정의하는 토폴로지입니다.
	// GRHISupportsRectTopology가 true인 경우에만 지원됩니다.
	PT_RectList,

	PT_Num,
	PT_NumBits = 3
};

// EPrimitiveType이 바이트 내에 들어가는지 확인하는 정적 어설션입니다.
static_assert(PT_Num <= (1 << 8), "EPrimitiveType doesn't fit in a byte");

// PT_NumBits가 너무 작은지 확인하는 정적 어설션입니다.
static_assert(PT_Num <= (1 << PT_NumBits), "PT_NumBits is too small");

enum ERasterizerFillMode : uint8
{
	FM_Point,
	FM_Wireframe,
	FM_Solid,

	ERasterizerFillMode_Num,
	ERasterizerFillMode_NumBits = 2,
};
static_assert(ERasterizerFillMode_Num <= (1 << ERasterizerFillMode_NumBits), "ERasterizerFillMode_Num will not fit on ERasterizerFillMode_NumBits");

enum ERasterizerCullMode : uint8
{
	CM_None,
	CM_CW,
	CM_CCW,

	ERasterizerCullMode_Num,
	ERasterizerCullMode_NumBits = 2,
};
static_assert(ERasterizerCullMode_Num <= (1 << ERasterizerCullMode_NumBits), "ERasterizerCullMode_Num will not fit on ERasterizerCullMode_NumBits");

enum class ERasterizerDepthClipMode : uint8
{
	DepthClip,
	DepthClamp,

	Num,
	NumBits = 1,
};
static_assert(uint32(ERasterizerDepthClipMode::Num) <= (1U << uint32(ERasterizerDepthClipMode::NumBits)), "ERasterizerDepthClipMode::Num will not fit on ERasterizerDepthClipMode::NumBits");

enum ERasterizerState
{
	E_SOLID_FRONT,
	E_SOLID_BACK,
	E_SOLID_NONE,

	E_WIREFRAME_FRONT,
	E_WIREFRAME_BACK,
	E_WIREFRAME_NONE,
};

enum ECompareFunction : uint8
{
	CF_Less,
	CF_LessEqual,
	CF_Greater,
	CF_GreaterEqual,
	CF_Equal,
	CF_NotEqual,
	CF_Never,
	CF_Always,

	ECompareFunction_Num,
	ECompareFunction_NumBits = 3,

	// Utility enumerations
	CF_DepthNearOrEqual = (((int32)ERHIZBuffer::IsInverted != 0) ? CF_GreaterEqual : CF_LessEqual),
	CF_DepthNear = (((int32)ERHIZBuffer::IsInverted != 0) ? CF_Greater : CF_Less),
	CF_DepthFartherOrEqual = (((int32)ERHIZBuffer::IsInverted != 0) ? CF_LessEqual : CF_GreaterEqual),
	CF_DepthFarther = (((int32)ERHIZBuffer::IsInverted != 0) ? CF_Less : CF_Greater),
};
static_assert(ECompareFunction_Num <= (1 << ECompareFunction_NumBits), "ECompareFunction_Num will not fit on ECompareFunction_NumBits");

enum EStencilMask
{
	SM_Default,
	SM_255,
	SM_1,
	SM_2,
	SM_4,
	SM_8,
	SM_16,
	SM_32,
	SM_64,
	SM_128,
	SM_Count
};

enum EStencilOp : uint8
{
	SO_Keep,
	SO_Zero,
	SO_Replace,
	SO_SaturatedIncrement,
	SO_SaturatedDecrement,
	SO_Invert,
	SO_Increment,
	SO_Decrement,

	EStencilOp_Num,
	EStencilOp_NumBits = 3,
};
static_assert(EStencilOp_Num <= (1 << EStencilOp_NumBits), "EStencilOp_Num will not fit on EStencilOp_NumBits");

enum ESamplerFilter : uint8
{
	SF_Point,
	SF_Bilinear,
	SF_Trilinear,
	SF_AnisotropicPoint,
	SF_AnisotropicLinear,

	ESamplerFilter_Num,
	ESamplerFilter_NumBits = 3,
};
static_assert(ESamplerFilter_Num <= (1 << ESamplerFilter_NumBits), "ESamplerFilter_Num will not fit on ESamplerFilter_NumBits");

enum ESamplerAddressMode : uint8
{
	AM_Wrap,
	AM_Clamp,
	AM_Mirror,
	/** Not supported on all platforms */
	AM_Border,

	ESamplerAddressMode_Num,
	ESamplerAddressMode_NumBits = 2,
};
static_assert(ESamplerAddressMode_Num <= (1 << ESamplerAddressMode_NumBits), "ESamplerAddressMode_Num will not fit on ESamplerAddressMode_NumBits");

enum ESamplerCompareFunction : uint8
{
	SCF_Never,
	SCF_Less
};
