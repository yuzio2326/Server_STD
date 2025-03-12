#pragma once
#include "CoreTypes.h"
#include "Math/SimpleMath.h"
#include "Templates/RefCounting.h"
#include "RHIDefinitions.h"
#include "RHIAccess.h"
#include "RHI.h"
#include "PixelFormat.h"

class FResourceBulkDataInterface;
extern RHI_API ERHIAccess RHIGetDefaultResourceState(ETextureCreateFlags InUsage, bool bInHasInitialData);

enum class EClearBinding
{
	ENoneBound, //no clear color associated with this target.  Target will not do hardware clears on most platforms
	EColorBound, //target has a clear color bound.  Clears will use the bound color, and do hardware clears.
	EDepthStencilBound, //target has a depthstencil value bound.  Clears will use the bound values and do hardware clears.
};

struct FClearValueBinding
{
	struct DSVAlue
	{
		float Depth;
		uint32 Stencil;
	};

	FClearValueBinding()
		: ColorBinding(EClearBinding::EColorBound)
	{
		Value.Color[0] = 0.0f;
		Value.Color[1] = 0.0f;
		Value.Color[2] = 0.0f;
		Value.Color[3] = 0.0f;
	}

	FClearValueBinding(EClearBinding NoBinding)
		: ColorBinding(NoBinding)
	{
		_ASSERT(ColorBinding == EClearBinding::ENoneBound);

		Value.Color[0] = 0.0f;
		Value.Color[1] = 0.0f;
		Value.Color[2] = 0.0f;
		Value.Color[3] = 0.0f;

		Value.DSValue.Depth = 0.0f;
		Value.DSValue.Stencil = 0;
	}

	explicit FClearValueBinding(const FLinearColor& InClearColor)
		: ColorBinding(EClearBinding::EColorBound)
	{
		Value.Color[0] = InClearColor.R();
		Value.Color[1] = InClearColor.G();
		Value.Color[2] = InClearColor.B();
		Value.Color[3] = InClearColor.A();
	}

	explicit FClearValueBinding(float DepthClearValue, uint32 StencilClearValue = 0)
		: ColorBinding(EClearBinding::EDepthStencilBound)
	{
		Value.DSValue.Depth = DepthClearValue;
		Value.DSValue.Stencil = StencilClearValue;
	}

	FLinearColor GetClearColor() const
	{
		_ASSERT(ColorBinding == EClearBinding::EColorBound);
		return FLinearColor(Value.Color[0], Value.Color[1], Value.Color[2], Value.Color[3]);
	}

	void GetDepthStencil(float& OutDepth, uint32& OutStencil) const
	{
		_ASSERT(ColorBinding == EClearBinding::EDepthStencilBound);
		OutDepth = Value.DSValue.Depth;
		OutStencil = Value.DSValue.Stencil;
	}

	bool operator==(const FClearValueBinding& Other) const
	{
		if (ColorBinding == Other.ColorBinding)
		{
			if (ColorBinding == EClearBinding::EColorBound)
			{
				return
					Value.Color[0] == Other.Value.Color[0] &&
					Value.Color[1] == Other.Value.Color[1] &&
					Value.Color[2] == Other.Value.Color[2] &&
					Value.Color[3] == Other.Value.Color[3];

			}
			if (ColorBinding == EClearBinding::EDepthStencilBound)
			{
				return
					Value.DSValue.Depth == Other.Value.DSValue.Depth &&
					Value.DSValue.Stencil == Other.Value.DSValue.Stencil;
			}
			return true;
		}
		return false;
	}

	/*friend inline uint32 GetTypeHash(FClearValueBinding const& Binding)
	{
		uint32 Hash = GetTypeHash(Binding.ColorBinding);

		if (Binding.ColorBinding == EClearBinding::EColorBound)
		{
			Hash = HashCombine(Hash, GetTypeHash(Binding.Value.Color[0]));
			Hash = HashCombine(Hash, GetTypeHash(Binding.Value.Color[1]));
			Hash = HashCombine(Hash, GetTypeHash(Binding.Value.Color[2]));
			Hash = HashCombine(Hash, GetTypeHash(Binding.Value.Color[3]));
		}
		else if (Binding.ColorBinding == EClearBinding::EDepthStencilBound)
		{
			Hash = HashCombine(Hash, GetTypeHash(Binding.Value.DSValue.Depth));
			Hash = HashCombine(Hash, GetTypeHash(Binding.Value.DSValue.Stencil));
		}

		return Hash;
	}*/

	EClearBinding ColorBinding;

	union ClearValueType
	{
		float Color[4];
		DSVAlue DSValue;
	} Value;

	// common clear values
	static RHI_API const FClearValueBinding None;
	static RHI_API const FClearValueBinding Black;
	static RHI_API const FClearValueBinding BlackMaxAlpha;
	static RHI_API const FClearValueBinding White;
	static RHI_API const FClearValueBinding Transparent;
	static RHI_API const FClearValueBinding DepthOne;
	static RHI_API const FClearValueBinding DepthZero;
	static RHI_API const FClearValueBinding DepthNear;
	static RHI_API const FClearValueBinding DepthFar;
	static RHI_API const FClearValueBinding Green;
	static RHI_API const FClearValueBinding DefaultNormal8Bit;
};


/** 텍스처 리소스를 생성하는 데 사용되는 디스크립터 */
struct FRHITextureDesc
{
	FRHITextureDesc() = default;

	FRHITextureDesc(const FRHITextureDesc& Other)
	{
		*this = Other;
	}

	FRHITextureDesc(ETextureDimension InDimension)
		: Dimension(InDimension)
	{
	}

	FRHITextureDesc(
		ETextureDimension   InDimension
		, ETextureCreateFlags InFlags
		, EPixelFormat        InFormat
		, FClearValueBinding  InClearValue
		, FVector2D           InExtent
		, uint16              InDepth
		, uint16              InArraySize
		, uint8               InNumMips
		, uint8               InNumSamples
		, uint32              InExtData
	)
		: Flags(InFlags)
		, ClearValue(InClearValue)
		, ExtData(InExtData)
		, Extent(InExtent)
		, Depth(InDepth)
		, ArraySize(InArraySize)
		, NumMips(InNumMips)
		, NumSamples(InNumSamples)
		, Dimension(InDimension)
		, Format(InFormat)
	{
	}

	/*friend uint32 GetTypeHash(const FRHITextureDesc& Desc)
	{
		uint32 Hash = GetTypeHash(Desc.Dimension);
		Hash = HashCombine(Hash, GetTypeHash(Desc.Flags));
		Hash = HashCombine(Hash, GetTypeHash(Desc.Format));
		Hash = HashCombine(Hash, GetTypeHash(Desc.UAVFormat));
		Hash = HashCombine(Hash, GetTypeHash(Desc.Extent));
		Hash = HashCombine(Hash, GetTypeHash(Desc.Depth));
		Hash = HashCombine(Hash, GetTypeHash(Desc.ArraySize));
		Hash = HashCombine(Hash, GetTypeHash(Desc.NumMips));
		Hash = HashCombine(Hash, GetTypeHash(Desc.NumSamples));
		Hash = HashCombine(Hash, GetTypeHash(Desc.FastVRAMPercentage));
		Hash = HashCombine(Hash, GetTypeHash(Desc.ClearValue));
		Hash = HashCombine(Hash, GetTypeHash(Desc.ExtData));
		Hash = HashCombine(Hash, GetTypeHash(Desc.GPUMask.GetNative()));
		return Hash;
	}*/

	bool operator == (const FRHITextureDesc& Other) const
	{
		return Dimension == Other.Dimension
			&& Flags == Other.Flags
			&& Format == Other.Format
			&& UAVFormat == Other.UAVFormat
			&& Extent == Other.Extent
			&& Depth == Other.Depth
			&& ArraySize == Other.ArraySize
			&& NumMips == Other.NumMips
			&& NumSamples == Other.NumSamples
			&& FastVRAMPercentage == Other.FastVRAMPercentage
			&& ClearValue == Other.ClearValue
			&& ExtData == Other.ExtData;
			//&& GPUMask == Other.GPUMask;
	}

	bool operator != (const FRHITextureDesc& Other) const
	{
		return !(*this == Other);
	}

	FRHITextureDesc& operator=(const FRHITextureDesc& Other)
	{
		Dimension = Other.Dimension;
		Flags = Other.Flags;
		Format = Other.Format;
		UAVFormat = Other.UAVFormat;
		Extent = Other.Extent;
		Depth = Other.Depth;
		ArraySize = Other.ArraySize;
		NumMips = Other.NumMips;
		NumSamples = Other.NumSamples;
		ClearValue = Other.ClearValue;
		ExtData = Other.ExtData;
		FastVRAMPercentage = Other.FastVRAMPercentage;
		//GPUMask = Other.GPUMask;

		return *this;
	}

	bool IsTexture2D() const
	{
		return Dimension == ETextureDimension::Texture2D || Dimension == ETextureDimension::Texture2DArray;
	}

	bool IsTexture3D() const
	{
		return Dimension == ETextureDimension::Texture3D;
	}

	bool IsTextureCube() const
	{
		return Dimension == ETextureDimension::TextureCube || Dimension == ETextureDimension::TextureCubeArray;
	}

	bool IsTextureArray() const
	{
		return Dimension == ETextureDimension::Texture2DArray || Dimension == ETextureDimension::TextureCubeArray;
	}

	bool IsMipChain() const
	{
		return NumMips > 1;
	}

	bool IsMultisample() const
	{
		return NumSamples > 1;
	}

	FVector3D GetSize() const
	{
		return FVector3D(Extent.x, Extent.y, Depth);
	}

	void Reset()
	{
		// Usually we don't want to propagate MSAA samples.
		NumSamples = 1;

		// Remove UAV flag for textures that don't need it (some formats are incompatible).
		Flags |= TexCreate_RenderTargetable;
		Flags &= ~(TexCreate_UAV | TexCreate_ResolveTargetable | TexCreate_DepthStencilResolveTarget | TexCreate_Memoryless);
	}

	/** Returns whether this descriptor conforms to requirements. */
	//bool IsValid() const
	//{
	//	return FRHITextureDesc::Validate(*this, /* Name = */ TEXT(""), /* bFatal = */ false);
	//}

	/** RHI 텍스처에 전달되는 텍스처 플래그 */
	ETextureCreateFlags Flags = TexCreate_None;

	/** 텍스처를 빠르게 클리어할 때 사용할 클리어 값 */
	FClearValueBinding ClearValue;

	/* 다중 GPU 시스템에서 리소스를 생성할 GPU를 나타내는 마스크 */
	//FRHIGPUMask GPUMask = FRHIGPUMask::All();

	/** 플랫폼별 추가 데이터. 일부 플랫폼에서는 오프라인 처리된 텍스처에 사용됩니다. */
	uint32 ExtData = 0;

	// 정수로 사용하십시오
	/** 텍스처의 x 및 y 차원 */
	FVector3D Extent = FVector3D(1.f, 1.f, 0.f);

	/** 차원이 3D인 경우 텍스처의 깊이 */
	uint16 Depth = 1;

	/** 텍스처의 배열 요소 수. (차원이 3D인 경우 1로 유지) */
	uint16 ArraySize = 1;

	/** 텍스처 미프맵 체인의 미프 수 */
	uint8 NumMips = 1;

	/** 텍스처의 샘플 수. MSAA의 경우 >1 */
	uint8 NumSamples = 1;

	/** RHI 텍스처를 생성할 때 사용할 텍스처 차원 */
	ETextureDimension Dimension = ETextureDimension::Texture2D;

	/** RHI 텍스처를 생성할 때 사용할 픽셀 포맷 */
	EPixelFormat Format = PF_Unknown;

	/** UAV를 생성할 때 사용할 텍스처 포맷. PF_Unknown은 기본 포맷을 사용함을 의미(Format과 동일) */
	EPixelFormat UAVFormat = PF_Unknown;

	/** 리소스 메모리의 몇 퍼센트를 빠른 VRAM에 할당할지(hint-only). (8비트 인코딩, 0..255 -> 0%..100%) */
	uint8 FastVRAMPercentage = 0xFF;

	/** 유효성을 확인합니다. */
	//static bool CheckValidity(const FRHITextureDesc& Desc, const TCHAR* Name)
	//{
	//	return FRHITextureDesc::Validate(Desc, Name, /* bFatal = */ true);
	//}

	///**
	// * 설명된 텍스처가 GPU 메모리에서 차지할 예상 총 메모리 크기를 반환합니다.
	// * 이 값은 텍스처의 크기/포맷 등을 고려한 추정치입니다.
	// *
	// * 실행 중인 플랫폼 RHI의 텍스처 리소스 크기를 정확히 측정하려면 RHICalcTexturePlatformSize()를 사용하십시오.
	// *
	// * @param FirstMipIndex - 메모리 크기 계산에 고려할 가장 상세한 mip의 인덱스. NumMips보다 작고 LastMipIndex보다 작거나 같아야 합니다.
	// * @param LastMipIndex  - 메모리 크기 계산에 고려할 가장 간단한 mip의 인덱스. NumMips보다 작고 FirstMipIndex보다 크거나 같아야 합니다.
	// */
	//RHI_API uint64 CalcMemorySizeEstimate(uint32 FirstMipIndex, uint32 LastMipIndex) const;


	//uint64 CalcMemorySizeEstimate(uint32 FirstMipIndex = 0) const
	//{
	//	return CalcMemorySizeEstimate(FirstMipIndex, NumMips - 1);
	//}

//private:
	//RHI_API static bool Validate(const FRHITextureDesc& Desc, const TCHAR* Name, bool bFatal);
};


struct FRHITextureCreateDesc : public FRHITextureDesc
{
	static FRHITextureCreateDesc Create(const TCHAR* InDebugName, ETextureDimension InDimension)
	{
		return FRHITextureCreateDesc(InDebugName, InDimension);
	}

	static FRHITextureCreateDesc Create2D(const TCHAR* InDebugName)
	{
		return FRHITextureCreateDesc(InDebugName, ETextureDimension::Texture2D);
	}

	static FRHITextureCreateDesc Create2DArray(const TCHAR* InDebugName)
	{
		return FRHITextureCreateDesc(InDebugName, ETextureDimension::Texture2DArray);
	}

	static FRHITextureCreateDesc Create3D(const TCHAR* InDebugName)
	{
		return FRHITextureCreateDesc(InDebugName, ETextureDimension::Texture3D);
	}

	static FRHITextureCreateDesc CreateCube(const TCHAR* InDebugName)
	{
		return FRHITextureCreateDesc(InDebugName, ETextureDimension::TextureCube);
	}

	static FRHITextureCreateDesc CreateCubeArray(const TCHAR* InDebugName)
	{
		return FRHITextureCreateDesc(InDebugName, ETextureDimension::TextureCubeArray);
	}

	static FRHITextureCreateDesc Create2D(const TCHAR* DebugName, FVector3D Size, EPixelFormat Format)
	{
		return Create2D(DebugName)
			.SetExtent(Size)
			.SetFormat(Format);
	}

	static FRHITextureCreateDesc Create2D(const TCHAR* DebugName, int32 SizeX, int32 SizeY, EPixelFormat Format)
	{
		return Create2D(DebugName)
			.SetExtent(SizeX, SizeY)
			.SetFormat(Format);
	}

	static FRHITextureCreateDesc Create2DArray(const TCHAR* DebugName, FVector3D Size, uint16 ArraySize, EPixelFormat Format)
	{
		return Create2DArray(DebugName)
			.SetExtent(Size)
			.SetFormat(Format)
			.SetArraySize((uint16)ArraySize);
	}

	static FRHITextureCreateDesc Create2DArray(const TCHAR* DebugName, int32 SizeX, int32 SizeY, int32 ArraySize, EPixelFormat Format)
	{
		return Create2DArray(DebugName)
			.SetExtent(SizeX, SizeY)
			.SetFormat(Format)
			.SetArraySize((uint16)ArraySize);
	}

	static FRHITextureCreateDesc Create3D(const TCHAR* DebugName, FVector3D Size, EPixelFormat Format)
	{
		return Create3D(DebugName)
			.SetExtent(Size.x, Size.y)
			.SetDepth((uint16)Size.z)
			.SetFormat(Format);
	}

	static FRHITextureCreateDesc Create3D(const TCHAR* DebugName, int32 SizeX, int32 SizeY, int32 SizeZ, EPixelFormat Format)
	{
		return Create3D(DebugName)
			.SetExtent(SizeX, SizeY)
			.SetDepth((uint16)SizeZ)
			.SetFormat(Format);
	}

	static FRHITextureCreateDesc CreateCube(const TCHAR* DebugName, uint32 Size, EPixelFormat Format)
	{
		return CreateCube(DebugName)
			.SetExtent(Size)
			.SetFormat(Format);
	}

	static FRHITextureCreateDesc CreateCubeArray(const TCHAR* DebugName, uint32 Size, uint16 ArraySize, EPixelFormat Format)
	{
		return CreateCubeArray(DebugName)
			.SetExtent(Size)
			.SetFormat(Format)
			.SetArraySize((uint16)ArraySize);
	}

	FRHITextureCreateDesc() = default;

	// Constructor with minimal argument set. Name and dimension are always required.
	FRHITextureCreateDesc(const TCHAR* InDebugName, ETextureDimension InDimension)
		: FRHITextureDesc(InDimension)
		, DebugName(InDebugName)
	{
	}

	// Constructor for when you already have an FRHITextureDesc
	FRHITextureCreateDesc(
		FRHITextureDesc const& InDesc
		, ERHIAccess                  InInitialState
		, TCHAR const* InDebugName
		, FResourceBulkDataInterface* InBulkData = nullptr
	)
		: FRHITextureDesc(InDesc)
		, InitialState(InInitialState)
		, DebugName(InDebugName)
		, BulkData(InBulkData)
	{
	}

	/*void CheckValidity() const
	{
		FRHITextureDesc::CheckValidity(*this, DebugName);

		ensureMsgf(InitialState != ERHIAccess::Unknown, TEXT("Resource %s cannot be created in an unknown state."), DebugName);
	}*/

	FRHITextureCreateDesc& SetFlags(ETextureCreateFlags InFlags) { Flags = InFlags;                          return *this; }
	FRHITextureCreateDesc& AddFlags(ETextureCreateFlags InFlags) { Flags |= InFlags;                         return *this; }
	FRHITextureCreateDesc& SetClearValue(FClearValueBinding InClearValue) { ClearValue = InClearValue;                return *this; }
	FRHITextureCreateDesc& SetExtData(uint32 InExtData) { ExtData = InExtData;                      return *this; }
	FRHITextureCreateDesc& SetExtent(const FVector3D& InExtent) { Extent = InExtent;                        return *this; }
	FRHITextureCreateDesc& SetExtent(int32 InExtentX, int32 InExtentY) { Extent = FVector3D(InExtentX, InExtentY, 0); return *this; }
	FRHITextureCreateDesc& SetExtent(uint32 InExtent) { Extent = FVector3D(InExtent, InExtent, 0);             return *this; }
	FRHITextureCreateDesc& SetDepth(uint16 InDepth) { Depth = InDepth;                          return *this; }
	FRHITextureCreateDesc& SetArraySize(uint16 InArraySize) { ArraySize = InArraySize;                  return *this; }
	FRHITextureCreateDesc& SetNumMips(uint8 InNumMips) { NumMips = InNumMips;                      return *this; }
	FRHITextureCreateDesc& SetNumSamples(uint8 InNumSamples) { NumSamples = InNumSamples;                return *this; }
	FRHITextureCreateDesc& SetDimension(ETextureDimension InDimension) { Dimension = InDimension;                  return *this; }
	FRHITextureCreateDesc& SetFormat(EPixelFormat InFormat) { Format = InFormat;                        return *this; }
	FRHITextureCreateDesc& SetUAVFormat(EPixelFormat InUAVFormat) { UAVFormat = InUAVFormat;                  return *this; }
	FRHITextureCreateDesc& SetInitialState(ERHIAccess InInitialState) { InitialState = InInitialState;            return *this; }
	FRHITextureCreateDesc& SetDebugName(const TCHAR* InDebugName) { DebugName = InDebugName;                  return *this; }
	//FRHITextureCreateDesc& SetGPUMask(FRHIGPUMask InGPUMask) { GPUMask = InGPUMask;                      return *this; }
	FRHITextureCreateDesc& SetBulkData(FResourceBulkDataInterface* InBulkData) { BulkData = InBulkData;                    return *this; }
	FRHITextureCreateDesc& DetermineInititialState() { if (InitialState == ERHIAccess::Unknown) InitialState = RHIGetDefaultResourceState(Flags, BulkData != nullptr); return *this; }
	FRHITextureCreateDesc& SetFastVRAMPercentage(float In) { FastVRAMPercentage = uint8(FMath::Clamp(In, 0.f, 1.0f) * 0xFF); return *this; }
	FRHITextureCreateDesc& SetClassName(const FName& InClassName) { ClassName = InClassName;				   return *this; }
	FRHITextureCreateDesc& SetOwnerName(const FName& InOwnerName) { OwnerName = InOwnerName;                  return *this; }
	//FName GetTraceClassName() const { const static FLazyName FRHITextureName(TEXT("FRHITexture")); return (ClassName == NAME_None) ? FRHITextureName : ClassName; }

	/* The RHI access state that the resource will be created in. */
	ERHIAccess InitialState = ERHIAccess::Unknown;

	/* A friendly name for the resource. */
	const TCHAR* DebugName = nullptr;

	/* Optional initial data to fill the resource with. */
	FResourceBulkDataInterface* BulkData = nullptr;

	FName ClassName = NAME_None;	// The owner class of FRHITexture used for Insight asset metadata tracing
	FName OwnerName = NAME_None;	// The owner name used for Insight asset metadata tracing
};


/** The base type of RHI resources. */
class FRHIResource
{
public:
	RHI_API FRHIResource(ERHIResourceType InResourceType);
	RHI_API virtual ~FRHIResource();

	inline uint32 AddRef() const
	{
		int32 NewValue = AtomicFlags.AddRef(std::memory_order_acquire);
		_ASSERT(NewValue > 0);
		return uint32(NewValue);
	}

private:
	// 강제 인라이닝을 방지하기 위한 별도의 함수입니다. 이는 코드 크기와 성능 모두에 도움이 됩니다.
	RHI_API void Destroy() const;

public:
	inline uint32 Release() const
	{
		int32 NewValue = AtomicFlags.Release(std::memory_order_release);
		_ASSERT(NewValue >= 0);

		if (NewValue == 0)
		{
			Destroy();
		}
		_ASSERT(NewValue >= 0);
		return uint32(NewValue);
	}

	inline uint32 GetRefCount() const
	{
		int32 CurrentValue = AtomicFlags.GetNumRefs(std::memory_order_relaxed);
		_ASSERT(CurrentValue >= 0);
		return uint32(CurrentValue);
	}

	bool IsValid() const
	{
		return AtomicFlags.IsValid(std::memory_order_relaxed);
	}

	void Delete()
	{
		AtomicFlags.MarkForDelete(std::memory_order_acquire);
		CurrentlyDeleting = this;
		delete this;
	}

	void DisableLifetimeExtension()
	{
		_ASSERT(IsValid(), TEXT("Resource is already marked for deletion. This call is a no-op. DisableLifetimeExtension must be called while still holding a live reference."));
		bAllowExtendLifetime = false;
	}

	inline ERHIResourceType GetType() const { return ResourceType; }

private:
	class FAtomicFlags
	{
		static constexpr ::uint32 MarkedForDeleteBit = 1 << 30;
		static constexpr ::uint32 DeletingBit = 1 << 31;
		static constexpr ::uint32 NumRefsMask = ~(MarkedForDeleteBit | DeletingBit);

		std::atomic_uint Packed = { 0 };

	public:
		int32 AddRef(std::memory_order MemoryOrder)
		{
			uint32 OldPacked = Packed.fetch_add(1, MemoryOrder);
			_ASSERT((OldPacked & DeletingBit) == 0, TEXT("Resource is being deleted."));
			int32  NumRefs = (OldPacked & NumRefsMask) + 1;
			_ASSERT(NumRefs < NumRefsMask, TEXT("Reference count has overflowed."));
			return NumRefs;
		}

		int32 Release(std::memory_order MemoryOrder)
		{
			uint32 OldPacked = Packed.fetch_sub(1, MemoryOrder);
			_ASSERT((OldPacked & DeletingBit) == 0, TEXT("Resource is being deleted."));
			int32  NumRefs = (OldPacked & NumRefsMask) - 1;
			_ASSERT(NumRefs >= 0, TEXT("Reference count has underflowed."));
			return NumRefs;
		}

		bool MarkForDelete(std::memory_order MemoryOrder)
		{
			uint32 OldPacked = Packed.fetch_or(MarkedForDeleteBit, MemoryOrder);
			_ASSERT((OldPacked & DeletingBit) == 0);
			return (OldPacked & MarkedForDeleteBit) != 0;
		}

		bool UnmarkForDelete(std::memory_order MemoryOrder)
		{
			uint32 OldPacked = Packed.fetch_xor(MarkedForDeleteBit, MemoryOrder);
			_ASSERT((OldPacked & DeletingBit) == 0);
			bool  OldMarkedForDelete = (OldPacked & MarkedForDeleteBit) != 0;
			_ASSERT(OldMarkedForDelete == true);
			return OldMarkedForDelete;
		}

		bool Deleteing()
		{
			uint32 LocalPacked = Packed.load(std::memory_order_acquire);
			_ASSERT((LocalPacked & MarkedForDeleteBit) != 0);
			_ASSERT((LocalPacked & DeletingBit) == 0);
			uint32 NumRefs = LocalPacked & NumRefsMask;

			if (NumRefs == 0) // caches can bring dead objects back to life
			{
//#if DO_CHECK
//				Packed.fetch_or(DeletingBit, std::memory_order_acquire);
//#endif
				return true;
			}
			else
			{
				UnmarkForDelete(std::memory_order_release);
				return false;
			}
		}

		bool IsValid(std::memory_order MemoryOrder)
		{
			uint32 LocalPacked = Packed.load(MemoryOrder);
			return (LocalPacked & MarkedForDeleteBit) == 0 && (LocalPacked & NumRefsMask) != 0;
		}

		bool IsMarkedForDelete(std::memory_order MemoryOrder)
		{
			return (Packed.load(MemoryOrder) & MarkedForDeleteBit) != 0;
		}

		int32 GetNumRefs(std::memory_order MemoryOrder)
		{
			return Packed.load(MemoryOrder) & NumRefsMask;
		}
	};
	mutable FAtomicFlags AtomicFlags;

	const ERHIResourceType ResourceType;
	uint8 bCommitted : 1;
	uint8 bAllowExtendLifetime : 1;

	RHI_API static FRHIResource* CurrentlyDeleting;

	friend FRHICommandList;
};

class FRHIViewport : public FRHIResource
{
public:
	FRHIViewport() : FRHIResource(RRT_Viewport) {}
	virtual FRHITexture* GetRenderTarget() const = 0;
	virtual void Resize(const uint32 NewSizeX, const uint32 NewSizeY) = 0;
};


class FRHIViewableResource : public FRHIResource
{
public:
	// TODO (RemoveUnknowns) FRHIBufferCreateDesc에 초기 접근이 포함되면 제거합니다.
	void SetTrackedAccess_Unsafe(ERHIAccess Access)
	{
		TrackedAccess = Access;
	}

	FName GetName() const
	{
		return Name;
	}

#if ENABLE_RHI_VALIDATION
	virtual RHIValidation::FResource* GetValidationTrackerResource() = 0;
#endif

protected:
	FRHIViewableResource(ERHIResourceType InResourceType, ERHIAccess InAccess)
		: FRHIResource(InResourceType)
		, TrackedAccess(InAccess)
	{
	}

	void TakeOwnership(FRHIViewableResource& Other)
	{
		TrackedAccess = Other.TrackedAccess;
	}

	void ReleaseOwnership()
	{
		TrackedAccess = ERHIAccess::Unknown;
	}

	FName Name;

private:
	ERHIAccess TrackedAccess;

	friend class FRHIComputeCommandList;
	friend class IRHIComputeContext;
};

class FRHITexture : public FRHIViewableResource
{
protected:
	/** Initialization constructor. Should only be called by platform RHI implementations. */
	RHI_API FRHITexture(const FRHITextureCreateDesc& InDesc);

public:
	/**
	 * 텍스처를 생성하는 데 사용된 텍스처 설명을 가져옵니다.
	 * 여전히 가상 함수로 남아 있는 이유는 FRHITextureReference가 이 함수를 재정의할 수 있기 때문입니다.
	 * FRHITextureReference가 더 이상 사용되지 않을 때 가상 키워드를 제거합니다.
	 *
	 * @return 텍스처를 생성하는 데 사용된 TextureDesc
	 */
	virtual const FRHITextureDesc& GetDesc() const { return TextureDesc; }

	///
	/// 각 RHI별로 구현된 가상 함수
	/// 

	virtual class FRHITextureReference* GetTextureReference() { return NULL; }
	//virtual FRHIDescriptorHandle GetDefaultBindlessHandle() const { return FRHIDescriptorHandle(); }

	/**
	 * 플랫폼별 네이티브 리소스 포인터에 접근을 반환합니다.
	 * 이는 플러그인에 기저 리소스에 대한 접근을 제공하기 위해 설계되었으며 매우 신중하게 또는 전혀 사용하지 않아야 합니다.
	 *
	 * @return 네이티브 리소스에 대한 포인터 또는 초기화되지 않았거나 이 리소스 타입에서 지원되지 않는 경우 NULL
	 */
	virtual void* GetNativeResource() const
	{
		// 파생 클래스에서 이 함수를 재정의하여 네이티브 텍스처 리소스에 대한 접근을 노출합니다.
		return nullptr;
	}

	/**
	 * 플랫폼별 네이티브 셰이더 리소스 뷰 포인터에 대한 접근을 반환합니다.
	 * 이는 플러그인에 기저 리소스에 대한 접근을 제공하기 위해 설계되었으며 매우 신중하게 또는 전혀 사용하지 않아야 합니다.
	 *
	 * @return 네이티브 리소스에 대한 포인터 또는 초기화되지 않았거나 이 리소스 타입에서 지원되지 않는 경우 NULL
	 */
	virtual void* GetNativeShaderResourceView() const
	{
		// 파생 클래스에서 이 함수를 재정의하여 네이티브 텍스처 리소스에 대한 접근을 노출합니다.
		return nullptr;
	}

	/**
	 * 플랫폼별 RHI 텍스처 기본 클래스에 대한 접근을 반환합니다.
	 * 이는 다중 상속 상황에서 RHI에 기본 클래스에 대한 빠른 접근을 제공하기 위해 설계되었습니다.
	 *
	 * @return 플랫폼별 RHI 텍스처 기본 클래스에 대한 포인터 또는 초기화되지 않았거나 이 RHI에서 지원되지 않는 경우 NULL
	 */
	virtual void* GetTextureBaseRHI()
	{
		// 파생 클래스에서 이 함수를 재정의하여 네이티브 텍스처 리소스에 대한 접근을 노출합니다.
		return nullptr;
	}

	virtual void GetWriteMaskProperties(void*& OutData, uint32& OutSize)
	{
		OutData = nullptr;
		OutSize = 0;
	}

	///
	/// 헬퍼 getter 함수 - 가상 아님
	/// 

	/**
	 * 텍스처의 x, y 및 z 차원을 반환합니다.
	 * Z 구성 요소는 항상 2D/큐브 리소스의 경우 1이고, 볼륨 텍스처의 깊이 및 배열 텍스처의 배열 크기를 나타냅니다.
	 */
	FVector3D GetSizeXYZ() const
	{
		const FRHITextureDesc& Desc = GetDesc();
		switch (Desc.Dimension)
		{
		case ETextureDimension::Texture2D:        return FVector3D(Desc.Extent.x, Desc.Extent.y, 1);
		case ETextureDimension::Texture2DArray:   return FVector3D(Desc.Extent.x, Desc.Extent.y, Desc.ArraySize);
		case ETextureDimension::Texture3D:        return FVector3D(Desc.Extent.x, Desc.Extent.y, Desc.Depth);
		case ETextureDimension::TextureCube:      return FVector3D(Desc.Extent.x, Desc.Extent.y, 1);
		case ETextureDimension::TextureCubeArray: return FVector3D(Desc.Extent.x, Desc.Extent.y, Desc.ArraySize);
		}
		return FVector3D(0, 0, 0);
	}

	/**
	 * 지정된 mip의 차원(즉, 각 차원에서 실제 텍셀 수)을 반환합니다. ArraySize는 무시됩니다.
	 * Z 구성 요소는 항상 2D/큐브 리소스의 경우 1이고, 볼륨 텍스처의 깊이를 나타냅니다.
	 * GetSizeXYZ()와 달리 2D 배열의 경우 Z에 ArraySize를 반환합니다.
	 */
	FVector3D GetMipDimensions(uint8 MipIndex) const
	{
		const FRHITextureDesc& Desc = GetDesc();
		return FVector3D(
			FMath::Max<int32>((int32)Desc.Extent.x >> MipIndex, 1),
			FMath::Max<int32>((int32)Desc.Extent.y >> MipIndex, 1),
			FMath::Max<int32>(Desc.Depth >> MipIndex, 1)
		);
	}

	/** @return 텍스처가 다중 샘플링되었는지 여부 */
	bool IsMultisampled() const { return GetDesc().NumSamples > 1; }

	/** @return 텍스처에 클리어 컬러가 정의되어 있는지 여부 */
	bool HasClearValue() const
	{
		return GetDesc().ClearValue.ColorBinding != EClearBinding::ENoneBound;
	}

	/** @return 설정된 경우 클리어 컬러 값 */
	FLinearColor GetClearColor() const
	{
		return GetDesc().ClearValue.GetClearColor();
	}

	/** @return 설정된 경우 깊이 및 스텐실 클리어 값 */
	void GetDepthStencilClearValue(float& OutDepth, uint32& OutStencil) const
	{
		return GetDesc().ClearValue.GetDepthStencil(OutDepth, OutStencil);
	}

	/** @return 설정된 경우 깊이 클리어 값 */
	float GetDepthClearValue() const
	{
		float Depth;
		uint32 Stencil;
		GetDesc().ClearValue.GetDepthStencil(Depth, Stencil);
		return Depth;
	}

	/** @return 설정된 경우 스텐실 클리어 값 */
	uint32 GetStencilClearValue() const
	{
		float Depth;
		uint32 Stencil;
		GetDesc().ClearValue.GetDepthStencil(Depth, Stencil);
		return Stencil;
	}

	///
	/// RenderTime 및 Name 함수 - 가상 아님
	/// 

	/** 이 텍스처가 리소스 테이블에 마지막으로 캐시된 시간을 설정합니다. */
	/*inline void SetLastRenderTime(float InLastRenderTime)
	{
		LastRenderTime.SetLastRenderTime(InLastRenderTime);
	}

	double GetLastRenderTime() const
	{
		return LastRenderTime.GetLastRenderTime();
	}*/

	RHI_API void SetName(const FName& InName);

	///
	/// 사용 중단된 함수들
	/// 

	//UE_DEPRECATED(5.1, "FRHITexture2D는 더 이상 사용되지 않습니다. FRHITexture를 직접 사용하십시오.")
	inline FRHITexture* GetTexture2D() { return TextureDesc.Dimension == ETextureDimension::Texture2D ? this : nullptr; }
	//UE_DEPRECATED(5.1, "FRHITexture2DArray는 더 이상 사용되지 않습니다. FRHITexture를 직접 사용하십시오.")
	//inline FRHITexture2DArray* GetTexture2DArray() { return TextureDesc.Dimension == ETextureDimension::Texture2DArray ? this : nullptr; }
	////UE_DEPRECATED(5.1, "FRHITexture3D는 더 이상 사용되지 않습니다. FRHITexture를 직접 사용하십시오.")
	//inline FRHITexture3D* GetTexture3D() { return TextureDesc.Dimension == ETextureDimension::Texture3D ? this : nullptr; }
	////UE_DEPRECATED(5.1, "FRHITextureCube는 더 이상 사용되지 않습니다. FRHITexture를 직접 사용하십시오.")
	//inline FRHITextureCube* GetTextureCube() { return TextureDesc.IsTextureCube() ? this : nullptr; }

	//UE_DEPRECATED(5.1, "GetSizeX()는 더 이상 사용되지 않습니다. GetDesc().Extent.X를 사용하십시오.")
	uint32 GetSizeX() const { return GetDesc().Extent.x; }

	//UE_DEPRECATED(5.1, "GetSizeY()는 더 이상 사용되지 않습니다. GetDesc().Extent.Y를 사용하십시오.")
	uint32 GetSizeY() const { return GetDesc().Extent.y; }

	//UE_DEPRECATED(5.1, "GetSizeXY()는 더 이상 사용되지 않습니다. GetDesc().Extent.X 또는 GetDesc().Extent.Y를 사용하십시오.")
	FVector3D GetSizeXY() const { return FVector3D(GetDesc().Extent.x, GetDesc().Extent.y, 0.f); }

	//UE_DEPRECATED(5.1, "GetSizeZ()는 더 이상 사용되지 않습니다. TextureArrays의 경우 GetDesc().ArraySize를, 3D 텍스처의 경우 GetDesc().Depth를 사용하십시오.")
	uint32 GetSizeZ() const { return GetSizeXYZ().z; }

	//UE_DEPRECATED(5.1, "GetNumMips()는 더 이상 사용되지 않습니다
		//UE_DEPRECATED(5.1, "GetNumMips()는 더 이상 사용되지 않습니다. GetDesc().NumMips를 사용하십시오.")
	uint32 GetNumMips() const { return GetDesc().NumMips; }

	//UE_DEPRECATED(5.1, "GetFormat()는 더 이상 사용되지 않습니다. GetDesc().Format을 사용하십시오.")
	EPixelFormat GetFormat() const { return GetDesc().Format; }

	//UE_DEPRECATED(5.1, "GetFlags()는 더 이상 사용되지 않습니다. GetDesc().Flags를 사용하십시오.")
	ETextureCreateFlags GetFlags() const { return GetDesc().Flags; }

	//UE_DEPRECATED(5.1, "GetNumSamples()는 더 이상 사용되지 않습니다. GetDesc().NumSamples를 사용하십시오.")
	uint32 GetNumSamples() const { return GetDesc().NumSamples; }

	//UE_DEPRECATED(5.1, "GetClearBinding()는 더 이상 사용되지 않습니다. GetDesc().ClearValue를 사용하십시오.")
	const FClearValueBinding GetClearBinding() const { return GetDesc().ClearValue; }

	//UE_DEPRECATED(5.1, "GetSize()는 더 이상 사용되지 않습니다. GetDesc().Extent.X를 사용하십시오.")
	uint32 GetSize() const { _ASSERT(GetDesc().IsTextureCube()); return GetDesc().Extent.x; }

private:
	FRHITextureDesc TextureDesc;
};

class FExclusiveDepthStencil
{
public:
	enum Type
	{
		// don't use those directly, use the combined versions below
		// 4 bits are used for depth and 4 for stencil to make the hex value readable and non overlapping
		DepthNop = 0x00,
		DepthRead = 0x01,
		DepthWrite = 0x02,
		DepthMask = 0x0f,
		StencilNop = 0x00,
		StencilRead = 0x10,
		StencilWrite = 0x20,
		StencilMask = 0xf0,

		// use those:
		DepthNop_StencilNop = DepthNop + StencilNop,
		DepthRead_StencilNop = DepthRead + StencilNop,
		DepthWrite_StencilNop = DepthWrite + StencilNop,
		DepthNop_StencilRead = DepthNop + StencilRead,
		DepthRead_StencilRead = DepthRead + StencilRead,
		DepthWrite_StencilRead = DepthWrite + StencilRead,
		DepthNop_StencilWrite = DepthNop + StencilWrite,
		DepthRead_StencilWrite = DepthRead + StencilWrite,
		DepthWrite_StencilWrite = DepthWrite + StencilWrite,
	};

private:
	Type Value;

public:
	// constructor
	FExclusiveDepthStencil(Type InValue = DepthNop_StencilNop)
		: Value(InValue)
	{
	}

	inline bool IsUsingDepthStencil() const
	{
		return Value != DepthNop_StencilNop;
	}
	inline bool IsUsingDepth() const
	{
		return (ExtractDepth() != DepthNop);
	}
	inline bool IsUsingStencil() const
	{
		return (ExtractStencil() != StencilNop);
	}
	inline bool IsDepthWrite() const
	{
		return ExtractDepth() == DepthWrite;
	}
	inline bool IsDepthRead() const
	{
		return ExtractDepth() == DepthRead;
	}
	inline bool IsStencilWrite() const
	{
		return ExtractStencil() == StencilWrite;
	}
	inline bool IsStencilRead() const
	{
		return ExtractStencil() == StencilRead;
	}

	inline bool IsAnyWrite() const
	{
		return IsDepthWrite() || IsStencilWrite();
	}

	inline void SetDepthWrite()
	{
		Value = (Type)(ExtractStencil() | DepthWrite);
	}
	inline void SetStencilWrite()
	{
		Value = (Type)(ExtractDepth() | StencilWrite);
	}
	inline void SetDepthStencilWrite(bool bDepth, bool bStencil)
	{
		Value = DepthNop_StencilNop;

		if (bDepth)
		{
			SetDepthWrite();
		}
		if (bStencil)
		{
			SetStencilWrite();
		}
	}
	bool operator==(const FExclusiveDepthStencil& rhs) const
	{
		return Value == rhs.Value;
	}

	bool operator != (const FExclusiveDepthStencil& RHS) const
	{
		return Value != RHS.Value;
	}

	inline bool IsValid(FExclusiveDepthStencil& Current) const
	{
		Type Depth = ExtractDepth();

		if (Depth != DepthNop && Depth != Current.ExtractDepth())
		{
			return false;
		}

		Type Stencil = ExtractStencil();

		if (Stencil != StencilNop && Stencil != Current.ExtractStencil())
		{
			return false;
		}

		return true;
	}

	inline void GetAccess(ERHIAccess& DepthAccess, ERHIAccess& StencilAccess) const
	{
		DepthAccess = ERHIAccess::None;

		// SRV access is allowed whilst a depth stencil target is "readable".
		constexpr ERHIAccess DSVReadOnlyMask =
			ERHIAccess::DSVRead;

		// If write access is required, only the depth block can access the resource.
		constexpr ERHIAccess DSVReadWriteMask =
			ERHIAccess::DSVRead |
			ERHIAccess::DSVWrite;

		if (IsUsingDepth())
		{
			DepthAccess = IsDepthWrite() ? DSVReadWriteMask : DSVReadOnlyMask;
		}

		StencilAccess = ERHIAccess::None;

		if (IsUsingStencil())
		{
			StencilAccess = IsStencilWrite() ? DSVReadWriteMask : DSVReadOnlyMask;
		}
	}

	//template <typename TFunction>
	//inline void EnumerateSubresources(TFunction Function) const
	//{
	//	if (!IsUsingDepthStencil())
	//	{
	//		return;
	//	}

	//	ERHIAccess DepthAccess = ERHIAccess::None;
	//	ERHIAccess StencilAccess = ERHIAccess::None;
	//	GetAccess(DepthAccess, StencilAccess);

	//	// Same depth / stencil state; single subresource.
	//	if (DepthAccess == StencilAccess)
	//	{
	//		Function(DepthAccess, FRHITransitionInfo::kAllSubresources);
	//	}
	//	// Separate subresources for depth / stencil.
	//	else
	//	{
	//		if (DepthAccess != ERHIAccess::None)
	//		{
	//			Function(DepthAccess, FRHITransitionInfo::kDepthPlaneSlice);
	//		}
	//		if (StencilAccess != ERHIAccess::None)
	//		{
	//			Function(StencilAccess, FRHITransitionInfo::kStencilPlaneSlice);
	//		}
	//	}
	//}

	/**
	* Returns a new FExclusiveDepthStencil to be used to transition a depth stencil resource to readable.
	* If the depth or stencil is already in a readable state, that particular component is returned as Nop,
	* to avoid unnecessary subresource transitions.
	*/
	inline FExclusiveDepthStencil GetReadableTransition() const
	{
		FExclusiveDepthStencil::Type NewDepthState = IsDepthWrite()
			? FExclusiveDepthStencil::DepthRead
			: FExclusiveDepthStencil::DepthNop;

		FExclusiveDepthStencil::Type NewStencilState = IsStencilWrite()
			? FExclusiveDepthStencil::StencilRead
			: FExclusiveDepthStencil::StencilNop;

		return (FExclusiveDepthStencil::Type)(NewDepthState | NewStencilState);
	}

	/**
	* Returns a new FExclusiveDepthStencil to be used to transition a depth stencil resource to readable.
	* If the depth or stencil is already in a readable state, that particular component is returned as Nop,
	* to avoid unnecessary subresource transitions.
	*/
	inline FExclusiveDepthStencil GetWritableTransition() const
	{
		FExclusiveDepthStencil::Type NewDepthState = IsDepthRead()
			? FExclusiveDepthStencil::DepthWrite
			: FExclusiveDepthStencil::DepthNop;

		FExclusiveDepthStencil::Type NewStencilState = IsStencilRead()
			? FExclusiveDepthStencil::StencilWrite
			: FExclusiveDepthStencil::StencilNop;

		return (FExclusiveDepthStencil::Type)(NewDepthState | NewStencilState);
	}

	uint32 GetIndex() const
	{
		// 참고: 배열을 인덱싱할 때 해당 순서로 뷰가 생성됩니다.

		// 우리는 Nop 버전에 신경 쓰지 않으므로 적은 뷰가 필요합니다.
		// 우리는 Nop과 Write를 결합합니다.
		switch (Value)
		{
		case DepthWrite_StencilNop:
		case DepthNop_StencilWrite:
		case DepthWrite_StencilWrite:
		case DepthNop_StencilNop:
			return 0; // 이전 DSAT_Writable

		case DepthRead_StencilNop:
		case DepthRead_StencilWrite:
			return 1; // 이전 DSAT_ReadOnlyDepth

		case DepthNop_StencilRead:
		case DepthWrite_StencilRead:
			return 2; // 이전 DSAT_ReadOnlyStencil

		case DepthRead_StencilRead:
			return 3; // 이전 DSAT_ReadOnlyDepthAndStencil
		}
		// 발생해서는 안 되는 경우
		_ASSERT(0);
		return -1;

	}
	static const uint32 MaxIndex = 4;

private:
	inline Type ExtractDepth() const
	{
		return (Type)(Value & DepthMask);
	}
	inline Type ExtractStencil() const
	{
		return (Type)(Value & StencilMask);
	}
	//friend uint32 GetTypeHash(const FExclusiveDepthStencil& Ds);
};


class FRHIRenderTargetView
{
public:
	FRHITexture* Texture = nullptr;
	uint32 MipIndex = 0;

	/** 배열 슬라이스 또는 텍스처 큐브 면. TexCreate_TargetArraySlicesIndependently로 텍스처 리소스가 생성된 경우에만 유효합니다! */
	uint32 ArraySliceIndex = -1;

	ERenderTargetLoadAction LoadAction = ERenderTargetLoadAction::ENoAction;
	ERenderTargetStoreAction StoreAction = ERenderTargetStoreAction::ENoAction;

	FRHIRenderTargetView() = default;
	FRHIRenderTargetView(FRHIRenderTargetView&&) = default;
	FRHIRenderTargetView(const FRHIRenderTargetView&) = default;
	FRHIRenderTargetView& operator=(FRHIRenderTargetView&&) = default;
	FRHIRenderTargetView& operator=(const FRHIRenderTargetView&) = default;

	//common case
	explicit FRHIRenderTargetView(FRHITexture* InTexture, ERenderTargetLoadAction InLoadAction) :
		Texture(InTexture),
		MipIndex(0),
		ArraySliceIndex(-1),
		LoadAction(InLoadAction),
		StoreAction(ERenderTargetStoreAction::EStore)
	{
	}

	//common case
	explicit FRHIRenderTargetView(FRHITexture* InTexture, ERenderTargetLoadAction InLoadAction, uint32 InMipIndex, uint32 InArraySliceIndex) :
		Texture(InTexture),
		MipIndex(InMipIndex),
		ArraySliceIndex(InArraySliceIndex),
		LoadAction(InLoadAction),
		StoreAction(ERenderTargetStoreAction::EStore)
	{
	}

	explicit FRHIRenderTargetView(FRHITexture* InTexture, uint32 InMipIndex, uint32 InArraySliceIndex, ERenderTargetLoadAction InLoadAction, ERenderTargetStoreAction InStoreAction) :
		Texture(InTexture),
		MipIndex(InMipIndex),
		ArraySliceIndex(InArraySliceIndex),
		LoadAction(InLoadAction),
		StoreAction(InStoreAction)
	{
	}

	bool operator==(const FRHIRenderTargetView& Other) const
	{
		return
			Texture == Other.Texture &&
			MipIndex == Other.MipIndex &&
			ArraySliceIndex == Other.ArraySliceIndex &&
			LoadAction == Other.LoadAction &&
			StoreAction == Other.StoreAction;
	}
};

class FRHIDepthRenderTargetView
{
public:
	FRHITexture* Texture;

	ERenderTargetLoadAction		DepthLoadAction;
	ERenderTargetStoreAction	DepthStoreAction;
	ERenderTargetLoadAction		StencilLoadAction;

private:
	ERenderTargetStoreAction	StencilStoreAction;
	FExclusiveDepthStencil		DepthStencilAccess;
public:

	// StencilStoreAction의 쓰기 접근을 방지하는 접근자
	ERenderTargetStoreAction GetStencilStoreAction() const { return StencilStoreAction; }
	// DepthStencilAccess의 쓰기 접근을 방지하는 접근자
	FExclusiveDepthStencil GetDepthStencilAccess() const { return DepthStencilAccess; }

	explicit FRHIDepthRenderTargetView() :
		Texture(nullptr),
		DepthLoadAction(ERenderTargetLoadAction::ENoAction),
		DepthStoreAction(ERenderTargetStoreAction::ENoAction),
		StencilLoadAction(ERenderTargetLoadAction::ENoAction),
		StencilStoreAction(ERenderTargetStoreAction::ENoAction),
		DepthStencilAccess(FExclusiveDepthStencil::DepthNop_StencilNop)
	{
		Validate();
	}

	//common case
	explicit FRHIDepthRenderTargetView(FRHITexture* InTexture, ERenderTargetLoadAction InLoadAction, ERenderTargetStoreAction InStoreAction) :
		Texture(InTexture),
		DepthLoadAction(InLoadAction),
		DepthStoreAction(InStoreAction),
		StencilLoadAction(InLoadAction),
		StencilStoreAction(InStoreAction),
		DepthStencilAccess(FExclusiveDepthStencil::DepthWrite_StencilWrite)
	{
		Validate();
	}

	explicit FRHIDepthRenderTargetView(FRHITexture* InTexture, ERenderTargetLoadAction InLoadAction, ERenderTargetStoreAction InStoreAction, FExclusiveDepthStencil InDepthStencilAccess) :
		Texture(InTexture),
		DepthLoadAction(InLoadAction),
		DepthStoreAction(InStoreAction),
		StencilLoadAction(InLoadAction),
		StencilStoreAction(InStoreAction),
		DepthStencilAccess(InDepthStencilAccess)
	{
		Validate();
	}

	explicit FRHIDepthRenderTargetView(FRHITexture* InTexture, ERenderTargetLoadAction InDepthLoadAction, ERenderTargetStoreAction InDepthStoreAction, ERenderTargetLoadAction InStencilLoadAction, ERenderTargetStoreAction InStencilStoreAction) :
		Texture(InTexture),
		DepthLoadAction(InDepthLoadAction),
		DepthStoreAction(InDepthStoreAction),
		StencilLoadAction(InStencilLoadAction),
		StencilStoreAction(InStencilStoreAction),
		DepthStencilAccess(FExclusiveDepthStencil::DepthWrite_StencilWrite)
	{
		Validate();
	}

	explicit FRHIDepthRenderTargetView(FRHITexture* InTexture, ERenderTargetLoadAction InDepthLoadAction, ERenderTargetStoreAction InDepthStoreAction, ERenderTargetLoadAction InStencilLoadAction, ERenderTargetStoreAction InStencilStoreAction, FExclusiveDepthStencil InDepthStencilAccess) :
		Texture(InTexture),
		DepthLoadAction(InDepthLoadAction),
		DepthStoreAction(InDepthStoreAction),
		StencilLoadAction(InStencilLoadAction),
		StencilStoreAction(InStencilStoreAction),
		DepthStencilAccess(InDepthStencilAccess)
	{
		Validate();
	}

	void Validate() const
	{
		// StoreAction이 DontCare인 경우 VK와 Metal은 첨부 파일을 정의되지 않은 상태로 남길 수 있습니다. 
		// 따라서 읽기 전용이 DontCare로 설정되어야 한다고 가정할 수 없습니다. 이 상태가 다시 사용되지 않을 것임을 확신할 수 없기 때문입니다.
		// ensureMsgf(DepthStencilAccess.IsDepthWrite() || DepthStoreAction == ERenderTargetStoreAction::ENoAction, TEXT("깊이가 읽기 전용이지만 저장 작업을 수행하고 있습니다. 이는 모바일에서 낭비입니다. 깊이가 변경될 수 없다면, 다시 저장할 필요가 없습니다."));
		/*ensureMsgf(DepthStencilAccess.IsStencilWrite() || StencilStoreAction == ERenderTargetStoreAction::ENoAction, TEXT("스텐실이 읽기 전용이지만 저장 작업을 수행하고 있습니다. 이는 모바일에서 낭비입니다. 스텐실이 변경될 수 없다면, 다시 저장할 필요가 없습니다."));*/
	}

	bool operator==(const FRHIDepthRenderTargetView& Other) const
	{
		return
			Texture == Other.Texture &&
			DepthLoadAction == Other.DepthLoadAction &&
			DepthStoreAction == Other.DepthStoreAction &&
			StencilLoadAction == Other.StencilLoadAction &&
			StencilStoreAction == Other.StencilStoreAction &&
			DepthStencilAccess == Other.DepthStencilAccess;
	}
};


class FRHISetRenderTargetsInfo
{
public:
	// Color Render Targets Info
	FRHIRenderTargetView ColorRenderTarget[MaxSimultaneousRenderTargets];
	int32 NumColorRenderTargets;
	bool bClearColor;

	// Color Render Targets Info
	FRHIRenderTargetView ColorResolveRenderTarget[MaxSimultaneousRenderTargets];
	bool bHasResolveAttachments;

	// Depth/Stencil Render Target Info
	FRHIDepthRenderTargetView DepthStencilRenderTarget;
	bool bClearDepth;
	bool bClearStencil;

	//FRHITexture* ShadingRateTexture;
	//EVRSRateCombiner ShadingRateTextureCombiner;

	uint8 MultiViewCount;

	FRHISetRenderTargetsInfo() :
		NumColorRenderTargets(0),
		bClearColor(false),
		bHasResolveAttachments(false),
		bClearDepth(false),
		//ShadingRateTexture(nullptr),
		MultiViewCount(0)
	{
	}

	FRHISetRenderTargetsInfo(int32 InNumColorRenderTargets, const FRHIRenderTargetView* InColorRenderTargets, const FRHIDepthRenderTargetView& InDepthStencilRenderTarget) :
		NumColorRenderTargets(InNumColorRenderTargets),
		bClearColor(InNumColorRenderTargets > 0 && InColorRenderTargets[0].LoadAction == ERenderTargetLoadAction::EClear),
		bHasResolveAttachments(false),
		DepthStencilRenderTarget(InDepthStencilRenderTarget),
		bClearDepth(InDepthStencilRenderTarget.Texture&& InDepthStencilRenderTarget.DepthLoadAction == ERenderTargetLoadAction::EClear)
		//ShadingRateTexture(nullptr),
		//ShadingRateTextureCombiner(VRSRB_Passthrough)
	{
		_ASSERT(InNumColorRenderTargets <= 0 || InColorRenderTargets);
		for (int32 Index = 0; Index < InNumColorRenderTargets; ++Index)
		{
			ColorRenderTarget[Index] = InColorRenderTargets[Index];
		}
	}
	// @todo metal mrt: This can go away after all the cleanup is done
	void SetClearDepthStencil(bool bInClearDepth, bool bInClearStencil = false)
	{
		if (bInClearDepth)
		{
			DepthStencilRenderTarget.DepthLoadAction = ERenderTargetLoadAction::EClear;
		}
		if (bInClearStencil)
		{
			DepthStencilRenderTarget.StencilLoadAction = ERenderTargetLoadAction::EClear;
		}
		bClearDepth = bInClearDepth;
		bClearStencil = bInClearStencil;
	}
};


enum class ERenderTargetActions : uint8
{
	LoadOpMask = 2,

#define RTACTION_MAKE_MASK(Load, Store) (((uint8)ERenderTargetLoadAction::Load << (uint8)LoadOpMask) | (uint8)ERenderTargetStoreAction::Store)

	DontLoad_DontStore = RTACTION_MAKE_MASK(ENoAction, ENoAction),

	DontLoad_Store = RTACTION_MAKE_MASK(ENoAction, EStore),
	Clear_Store = RTACTION_MAKE_MASK(EClear, EStore),
	Load_Store = RTACTION_MAKE_MASK(ELoad, EStore),

	Clear_DontStore = RTACTION_MAKE_MASK(EClear, ENoAction),
	Load_DontStore = RTACTION_MAKE_MASK(ELoad, ENoAction),
	Clear_Resolve = RTACTION_MAKE_MASK(EClear, EMultisampleResolve),
	Load_Resolve = RTACTION_MAKE_MASK(ELoad, EMultisampleResolve),

#undef RTACTION_MAKE_MASK
};

inline ERenderTargetActions MakeRenderTargetActions(ERenderTargetLoadAction Load, ERenderTargetStoreAction Store)
{
	return (ERenderTargetActions)(((uint8)Load << (uint8)ERenderTargetActions::LoadOpMask) | (uint8)Store);
}

inline ERenderTargetLoadAction GetLoadAction(ERenderTargetActions Action)
{
	return (ERenderTargetLoadAction)((uint8)Action >> (uint8)ERenderTargetActions::LoadOpMask);
}

inline ERenderTargetStoreAction GetStoreAction(ERenderTargetActions Action)
{
	return (ERenderTargetStoreAction)((uint8)Action & ((1 << (uint8)ERenderTargetActions::LoadOpMask) - 1));
}

enum class EDepthStencilTargetActions : uint8
{
	DepthMask = 4,

#define RTACTION_MAKE_MASK(Depth, Stencil) (((uint8)ERenderTargetActions::Depth << (uint8)DepthMask) | (uint8)ERenderTargetActions::Stencil)

	DontLoad_DontStore = RTACTION_MAKE_MASK(DontLoad_DontStore, DontLoad_DontStore),
	DontLoad_StoreDepthStencil = RTACTION_MAKE_MASK(DontLoad_Store, DontLoad_Store),
	DontLoad_StoreStencilNotDepth = RTACTION_MAKE_MASK(DontLoad_DontStore, DontLoad_Store),
	ClearDepthStencil_StoreDepthStencil = RTACTION_MAKE_MASK(Clear_Store, Clear_Store),
	LoadDepthStencil_StoreDepthStencil = RTACTION_MAKE_MASK(Load_Store, Load_Store),
	LoadDepthNotStencil_StoreDepthNotStencil = RTACTION_MAKE_MASK(Load_Store, DontLoad_DontStore),
	LoadDepthNotStencil_DontStore = RTACTION_MAKE_MASK(Load_DontStore, DontLoad_DontStore),
	LoadDepthStencil_StoreStencilNotDepth = RTACTION_MAKE_MASK(Load_DontStore, Load_Store),

	ClearDepthStencil_DontStoreDepthStencil = RTACTION_MAKE_MASK(Clear_DontStore, Clear_DontStore),
	LoadDepthStencil_DontStoreDepthStencil = RTACTION_MAKE_MASK(Load_DontStore, Load_DontStore),
	ClearDepthStencil_StoreDepthNotStencil = RTACTION_MAKE_MASK(Clear_Store, Clear_DontStore),
	ClearDepthStencil_StoreStencilNotDepth = RTACTION_MAKE_MASK(Clear_DontStore, Clear_Store),
	ClearDepthStencil_ResolveDepthNotStencil = RTACTION_MAKE_MASK(Clear_Resolve, Clear_DontStore),
	ClearDepthStencil_ResolveStencilNotDepth = RTACTION_MAKE_MASK(Clear_DontStore, Clear_Resolve),
	LoadDepthClearStencil_StoreDepthStencil = RTACTION_MAKE_MASK(Load_Store, Clear_Store),

	ClearStencilDontLoadDepth_StoreStencilNotDepth = RTACTION_MAKE_MASK(DontLoad_DontStore, Clear_Store),

#undef RTACTION_MAKE_MASK
};

inline constexpr EDepthStencilTargetActions MakeDepthStencilTargetActions(const ERenderTargetActions Depth, const ERenderTargetActions Stencil)
{
	return (EDepthStencilTargetActions)(((uint8)Depth << (uint8)EDepthStencilTargetActions::DepthMask) | (uint8)Stencil);
}

inline ERenderTargetActions GetDepthActions(EDepthStencilTargetActions Action)
{
	return (ERenderTargetActions)((uint8)Action >> (uint8)EDepthStencilTargetActions::DepthMask);
}

inline ERenderTargetActions GetStencilActions(EDepthStencilTargetActions Action)
{
	return (ERenderTargetActions)((uint8)Action & ((1 << (uint8)EDepthStencilTargetActions::DepthMask) - 1));
}

struct FRHIRenderPassInfo
{
	struct FColorEntry
	{
		FRHITexture* RenderTarget = nullptr;
		FRHITexture* ResolveTarget = nullptr;
		int32                ArraySlice = -1;
		uint8                MipIndex = 0;
		ERenderTargetActions Action = ERenderTargetActions::DontLoad_DontStore;
	};
	TStaticArray<FColorEntry, MaxSimultaneousRenderTargets> ColorRenderTargets;

	struct FDepthStencilEntry
	{
		FRHITexture* DepthStencilTarget = nullptr;
		FRHITexture* ResolveTarget = nullptr;
		EDepthStencilTargetActions Action = EDepthStencilTargetActions::DontLoad_DontStore;
		FExclusiveDepthStencil ExclusiveDepthStencil;
	};
	FDepthStencilEntry DepthStencilRenderTarget;

	// 멀티샘플 해상도 또는 래스터 UAV(즉, 고정 기능 타겟 아님) 작업의 영역을 제어합니다.
	//FResolveRect ResolveRect;

	// Some RHIs can use a texture to control the sampling and/or shading resolution of different areas 
	//FTextureRHIRef ShadingRateTexture = nullptr;
	//EVRSRateCombiner ShadingRateTextureCombiner = VRSRB_Passthrough;

	// Some RHIs require a hint that occlusion queries will be used in this render pass
	uint32 NumOcclusionQueries = 0;
	bool bOcclusionQueries = false;

	// if this renderpass should be multiview, and if so how many views are required
	uint8 MultiViewCount = 0;

	// Hint for some RHI's that renderpass will have specific sub-passes 
	//ESubpassHint SubpassHint = ESubpassHint::None;

	FRHIRenderPassInfo() = default;
	FRHIRenderPassInfo(const FRHIRenderPassInfo&) = default;
	FRHIRenderPassInfo& operator=(const FRHIRenderPassInfo&) = default;

	// Color, no depth, optional resolve, optional mip, optional array slice
	explicit FRHIRenderPassInfo(FRHITexture* ColorRT, ERenderTargetActions ColorAction, FRHITexture* ResolveRT = nullptr, uint8 InMipIndex = 0, int32 InArraySlice = -1)
	{
		_ASSERT(!(ResolveRT && ResolveRT->IsMultisampled()));
		_ASSERT(ColorRT);
		ColorRenderTargets[0].RenderTarget = ColorRT;
		ColorRenderTargets[0].ResolveTarget = ResolveRT;
		ColorRenderTargets[0].ArraySlice = InArraySlice;
		ColorRenderTargets[0].MipIndex = InMipIndex;
		ColorRenderTargets[0].Action = ColorAction;
	}

	// Color MRTs, no depth
	explicit FRHIRenderPassInfo(int32 NumColorRTs, FRHITexture* ColorRTs[], ERenderTargetActions ColorAction)
	{
		_ASSERT(NumColorRTs > 0);
		for (int32 Index = 0; Index < NumColorRTs; ++Index)
		{
			_ASSERT(ColorRTs[Index]);
			ColorRenderTargets[Index].RenderTarget = ColorRTs[Index];
			ColorRenderTargets[Index].ArraySlice = -1;
			ColorRenderTargets[Index].Action = ColorAction;
		}
		DepthStencilRenderTarget.DepthStencilTarget = nullptr;
		DepthStencilRenderTarget.Action = EDepthStencilTargetActions::DontLoad_DontStore;
		DepthStencilRenderTarget.ExclusiveDepthStencil = FExclusiveDepthStencil::DepthNop_StencilNop;
		DepthStencilRenderTarget.ResolveTarget = nullptr;
	}

	// Color MRTs, no depth
	explicit FRHIRenderPassInfo(int32 NumColorRTs, FRHITexture* ColorRTs[], ERenderTargetActions ColorAction, FRHITexture* ResolveTargets[])
	{
		_ASSERT(NumColorRTs > 0);
		for (int32 Index = 0; Index < NumColorRTs; ++Index)
		{
			_ASSERT(ColorRTs[Index]);
			ColorRenderTargets[Index].RenderTarget = ColorRTs[Index];
			ColorRenderTargets[Index].ResolveTarget = ResolveTargets[Index];
			ColorRenderTargets[Index].ArraySlice = -1;
			ColorRenderTargets[Index].MipIndex = 0;
			ColorRenderTargets[Index].Action = ColorAction;
		}
		DepthStencilRenderTarget.DepthStencilTarget = nullptr;
		DepthStencilRenderTarget.Action = EDepthStencilTargetActions::DontLoad_DontStore;
		DepthStencilRenderTarget.ExclusiveDepthStencil = FExclusiveDepthStencil::DepthNop_StencilNop;
		DepthStencilRenderTarget.ResolveTarget = nullptr;
	}

	// Color MRTs and depth
	explicit FRHIRenderPassInfo(int32 NumColorRTs, FRHITexture* ColorRTs[], ERenderTargetActions ColorAction, FRHITexture* DepthRT, EDepthStencilTargetActions DepthActions, FExclusiveDepthStencil InEDS = FExclusiveDepthStencil::DepthWrite_StencilWrite)
	{
		_ASSERT(NumColorRTs > 0);
		for (int32 Index = 0; Index < NumColorRTs; ++Index)
		{
			_ASSERT(ColorRTs[Index]);
			ColorRenderTargets[Index].RenderTarget = ColorRTs[Index];
			ColorRenderTargets[Index].ResolveTarget = nullptr;
			ColorRenderTargets[Index].ArraySlice = -1;
			ColorRenderTargets[Index].MipIndex = 0;
			ColorRenderTargets[Index].Action = ColorAction;
		}
		_ASSERT(DepthRT);
		DepthStencilRenderTarget.DepthStencilTarget = DepthRT;
		DepthStencilRenderTarget.ResolveTarget = nullptr;
		DepthStencilRenderTarget.Action = DepthActions;
		DepthStencilRenderTarget.ExclusiveDepthStencil = InEDS;
	}

	// Color MRTs and depth
	explicit FRHIRenderPassInfo(int32 NumColorRTs, FRHITexture* ColorRTs[], ERenderTargetActions ColorAction, FRHITexture* ResolveRTs[], FRHITexture* DepthRT, EDepthStencilTargetActions DepthActions, FRHITexture* ResolveDepthRT, FExclusiveDepthStencil InEDS = FExclusiveDepthStencil::DepthWrite_StencilWrite)
	{
		_ASSERT(NumColorRTs > 0);
		for (int32 Index = 0; Index < NumColorRTs; ++Index)
		{
			_ASSERT(!ResolveRTs[Index] || ResolveRTs[Index]->IsMultisampled());
			_ASSERT(ColorRTs[Index]);
			ColorRenderTargets[Index].RenderTarget = ColorRTs[Index];
			ColorRenderTargets[Index].ResolveTarget = ResolveRTs[Index];
			ColorRenderTargets[Index].ArraySlice = -1;
			ColorRenderTargets[Index].MipIndex = 0;
			ColorRenderTargets[Index].Action = ColorAction;
		}
		_ASSERT(!ResolveDepthRT || ResolveDepthRT->IsMultisampled());
		_ASSERT(DepthRT);
		DepthStencilRenderTarget.DepthStencilTarget = DepthRT;
		DepthStencilRenderTarget.ResolveTarget = ResolveDepthRT;
		DepthStencilRenderTarget.Action = DepthActions;
		DepthStencilRenderTarget.ExclusiveDepthStencil = InEDS;
	}

	// Depth, no color
	explicit FRHIRenderPassInfo(FRHITexture* DepthRT, EDepthStencilTargetActions DepthActions, FRHITexture* ResolveDepthRT = nullptr, FExclusiveDepthStencil InEDS = FExclusiveDepthStencil::DepthWrite_StencilWrite)
	{
		_ASSERT(!ResolveDepthRT || ResolveDepthRT->IsMultisampled());
		_ASSERT(DepthRT);
		DepthStencilRenderTarget.DepthStencilTarget = DepthRT;
		DepthStencilRenderTarget.ResolveTarget = ResolveDepthRT;
		DepthStencilRenderTarget.Action = DepthActions;
		DepthStencilRenderTarget.ExclusiveDepthStencil = InEDS;
	}

	// Depth, no color, occlusion queries
	explicit FRHIRenderPassInfo(FRHITexture* DepthRT, uint32 InNumOcclusionQueries, EDepthStencilTargetActions DepthActions, FRHITexture* ResolveDepthRT = nullptr, FExclusiveDepthStencil InEDS = FExclusiveDepthStencil::DepthWrite_StencilWrite)
		: NumOcclusionQueries(InNumOcclusionQueries)
	{
		_ASSERT(!ResolveDepthRT || ResolveDepthRT->IsMultisampled());
		_ASSERT(DepthRT);
		DepthStencilRenderTarget.DepthStencilTarget = DepthRT;
		DepthStencilRenderTarget.ResolveTarget = ResolveDepthRT;
		DepthStencilRenderTarget.Action = DepthActions;
		DepthStencilRenderTarget.ExclusiveDepthStencil = InEDS;
	}

	// Color and depth
	explicit FRHIRenderPassInfo(FRHITexture* ColorRT, ERenderTargetActions ColorAction, FRHITexture* DepthRT, EDepthStencilTargetActions DepthActions, FExclusiveDepthStencil InEDS = FExclusiveDepthStencil::DepthWrite_StencilWrite)
	{
		_ASSERT(ColorRT);
		ColorRenderTargets[0].RenderTarget = ColorRT;
		ColorRenderTargets[0].ResolveTarget = nullptr;
		ColorRenderTargets[0].ArraySlice = -1;
		ColorRenderTargets[0].MipIndex = 0;
		ColorRenderTargets[0].Action = ColorAction;
		_ASSERT(DepthRT);
		DepthStencilRenderTarget.DepthStencilTarget = DepthRT;
		DepthStencilRenderTarget.ResolveTarget = nullptr;
		DepthStencilRenderTarget.Action = DepthActions;
		DepthStencilRenderTarget.ExclusiveDepthStencil = InEDS;
		ZeroMemory(&ColorRenderTargets[1], sizeof(FColorEntry) * (MaxSimultaneousRenderTargets - 1));
	}

	// Color and depth with resolve
	explicit FRHIRenderPassInfo(FRHITexture* ColorRT, ERenderTargetActions ColorAction, FRHITexture* ResolveColorRT,
		FRHITexture* DepthRT, EDepthStencilTargetActions DepthActions, FRHITexture* ResolveDepthRT, FExclusiveDepthStencil InEDS = FExclusiveDepthStencil::DepthWrite_StencilWrite)
	{
		_ASSERT(!ResolveColorRT || ResolveColorRT->IsMultisampled());
		_ASSERT(!ResolveDepthRT || ResolveDepthRT->IsMultisampled());
		_ASSERT(ColorRT);
		ColorRenderTargets[0].RenderTarget = ColorRT;
		ColorRenderTargets[0].ResolveTarget = ResolveColorRT;
		ColorRenderTargets[0].ArraySlice = -1;
		ColorRenderTargets[0].MipIndex = 0;
		ColorRenderTargets[0].Action = ColorAction;
		_ASSERT(DepthRT);
		DepthStencilRenderTarget.DepthStencilTarget = DepthRT;
		DepthStencilRenderTarget.ResolveTarget = ResolveDepthRT;
		DepthStencilRenderTarget.Action = DepthActions;
		DepthStencilRenderTarget.ExclusiveDepthStencil = InEDS;
		ZeroMemory(&ColorRenderTargets[1], sizeof(FColorEntry) * (MaxSimultaneousRenderTargets - 1));
	}

	// Color and depth with resolve and optional sample density
	/*explicit FRHIRenderPassInfo(FRHITexture* ColorRT, ERenderTargetActions ColorAction, FRHITexture* ResolveColorRT,
		FRHITexture* DepthRT, EDepthStencilTargetActions DepthActions, FRHITexture* ResolveDepthRT,
		FRHITexture* InShadingRateTexture, EVRSRateCombiner InShadingRateTextureCombiner,
		FExclusiveDepthStencil InEDS = FExclusiveDepthStencil::DepthWrite_StencilWrite)
	{
		check(!ResolveColorRT || ResolveColorRT->IsMultisampled());
		check(!ResolveDepthRT || ResolveDepthRT->IsMultisampled());
		check(ColorRT);
		ColorRenderTargets[0].RenderTarget = ColorRT;
		ColorRenderTargets[0].ResolveTarget = ResolveColorRT;
		ColorRenderTargets[0].ArraySlice = -1;
		ColorRenderTargets[0].MipIndex = 0;
		ColorRenderTargets[0].Action = ColorAction;
		check(DepthRT);
		DepthStencilRenderTarget.DepthStencilTarget = DepthRT;
		DepthStencilRenderTarget.ResolveTarget = ResolveDepthRT;
		DepthStencilRenderTarget.Action = DepthActions;
		DepthStencilRenderTarget.ExclusiveDepthStencil = InEDS;
		ShadingRateTexture = InShadingRateTexture;
		ShadingRateTextureCombiner = InShadingRateTextureCombiner;
		FMemory::Memzero(&ColorRenderTargets[1], sizeof(FColorEntry) * (MaxSimultaneousRenderTargets - 1));
	}*/

	inline int32 GetNumColorRenderTargets() const
	{
		int32 ColorIndex = 0;
		for (; ColorIndex < MaxSimultaneousRenderTargets; ++ColorIndex)
		{
			const FColorEntry& Entry = ColorRenderTargets[ColorIndex];
			if (!Entry.RenderTarget)
			{
				break;
			}
		}

		return ColorIndex;
	}

	/*FGraphicsPipelineRenderTargetsInfo ExtractRenderTargetsInfo() const
	{
		FGraphicsPipelineRenderTargetsInfo RenderTargetsInfo;

		RenderTargetsInfo.NumSamples = 1;
		int32 RenderTargetIndex = 0;

		for (; RenderTargetIndex < MaxSimultaneousRenderTargets; ++RenderTargetIndex)
		{
			FRHITexture* RenderTarget = ColorRenderTargets[RenderTargetIndex].RenderTarget;
			if (!RenderTarget)
			{
				break;
			}

			RenderTargetsInfo.RenderTargetFormats[RenderTargetIndex] = (uint8)RenderTarget->GetFormat();
			RenderTargetsInfo.RenderTargetFlags[RenderTargetIndex] = RenderTarget->GetFlags();
			RenderTargetsInfo.NumSamples |= RenderTarget->GetNumSamples();
		}

		RenderTargetsInfo.RenderTargetsEnabled = RenderTargetIndex;
		for (; RenderTargetIndex < MaxSimultaneousRenderTargets; ++RenderTargetIndex)
		{
			RenderTargetsInfo.RenderTargetFormats[RenderTargetIndex] = PF_Unknown;
		}

		if (DepthStencilRenderTarget.DepthStencilTarget)
		{
			RenderTargetsInfo.DepthStencilTargetFormat = DepthStencilRenderTarget.DepthStencilTarget->GetFormat();
			RenderTargetsInfo.DepthStencilTargetFlag = DepthStencilRenderTarget.DepthStencilTarget->GetFlags();
			RenderTargetsInfo.NumSamples |= DepthStencilRenderTarget.DepthStencilTarget->GetNumSamples();
		}
		else
		{
			RenderTargetsInfo.DepthStencilTargetFormat = PF_Unknown;
		}

		const ERenderTargetActions DepthActions = GetDepthActions(DepthStencilRenderTarget.Action);
		const ERenderTargetActions StencilActions = GetStencilActions(DepthStencilRenderTarget.Action);
		RenderTargetsInfo.DepthTargetLoadAction = GetLoadAction(DepthActions);
		RenderTargetsInfo.DepthTargetStoreAction = GetStoreAction(DepthActions);
		RenderTargetsInfo.StencilTargetLoadAction = GetLoadAction(StencilActions);
		RenderTargetsInfo.StencilTargetStoreAction = GetStoreAction(StencilActions);
		RenderTargetsInfo.DepthStencilAccess = DepthStencilRenderTarget.ExclusiveDepthStencil;

		RenderTargetsInfo.MultiViewCount = MultiViewCount;
		RenderTargetsInfo.bHasFragmentDensityAttachment = ShadingRateTexture != nullptr;

		return RenderTargetsInfo;
	}*/

//#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
//	RHI_API void Validate() const;
//#else
//	void Validate() const {}
//#endif
	RHI_API void ConvertToRenderTargetsInfo(FRHISetRenderTargetsInfo& OutRTInfo) const;
};


class RHI_API FRHIShader : public FRHIResource
{
public:
	const TCHAR* GetShaderName() const { return TEXT("<unknown>"); }
	FString GetUniformBufferName(uint32 Index) const { return TEXT("<unknown>"); }

	FRHIShader() = delete;
	FRHIShader(ERHIResourceType InResourceType, EShaderFrequency InFrequency)
		: FRHIResource(InResourceType)
		, Frequency(InFrequency)
	{
	}

	virtual ~FRHIShader();

	inline EShaderFrequency GetFrequency() const
	{
		return Frequency;
	}

private:
	EShaderFrequency Frequency;
};

class FRHIGraphicsShader : public FRHIShader
{
public:
	explicit FRHIGraphicsShader(ERHIResourceType InResourceType, EShaderFrequency InFrequency)
		: FRHIShader(InResourceType, InFrequency) { }
};

class FRHIVertexShader : public FRHIGraphicsShader
{
public:
	FRHIVertexShader() : FRHIGraphicsShader(RRT_VertexShader, SF_Vertex) {}
};

class FRHIPixelShader : public FRHIGraphicsShader
{
public:
	FRHIPixelShader() : FRHIGraphicsShader(RRT_PixelShader, SF_Pixel) {}
};

class FRHIVertexDeclaration : public FRHIResource
{
public:
	FRHIVertexDeclaration() : FRHIResource(RRT_VertexDeclaration) {}
	virtual bool GetInitializer(FVertexDeclarationElementList& Init) { return false; }
	//virtual uint32 GetPrecachePSOHash() const { return 0; }

	uint32 Hash = 0;
};

class FRHIBoundShaderState : public FRHIResource
{
public:
	FRHIBoundShaderState() : FRHIResource(RRT_BoundShaderState) {}
};

class FResourceBulkDataInterface;
class FResourceArrayInterface;

struct FRHIResourceCreateInfo
{
	FRHIResourceCreateInfo(const TCHAR* InDebugName)
		: BulkData(nullptr)
		, ResourceArray(nullptr)
		, ClearValueBinding(FLinearColor::Transparent)
		//, GPUMask(FRHIGPUMask::All())
		, bWithoutNativeResource(false)
		, DebugName(InDebugName)
		, ExtData(0)
	{
		_ASSERT(InDebugName);
	}

	// 텍스처 생성을 위한 호출
	FRHIResourceCreateInfo(const TCHAR* InDebugName, FResourceBulkDataInterface* InBulkData)
		: FRHIResourceCreateInfo(InDebugName)
	{
		BulkData = InBulkData;
	}

	// 버퍼 생성을 위한 호출
	FRHIResourceCreateInfo(const TCHAR* InDebugName, FResourceArrayInterface* InResourceArray)
		: FRHIResourceCreateInfo(InDebugName)
	{
		ResourceArray = InResourceArray;
	}

	FRHIResourceCreateInfo(const TCHAR* InDebugName, const FClearValueBinding& InClearValueBinding)
		: FRHIResourceCreateInfo(InDebugName)
	{
		ClearValueBinding = InClearValueBinding;
	}

	FRHIResourceCreateInfo(uint32 InExtData)
		: FRHIResourceCreateInfo(TEXT(""))
	{
		ExtData = InExtData;
	}

	//FName GetTraceClassName() const { const static FLazyName FRHIBufferName(TEXT("FRHIBuffer")); return (ClassName == NAME_None) ? FRHIBufferName : ClassName; }

	// 텍스처 생성을 위한 호출
	FResourceBulkDataInterface* BulkData;

	// 버퍼 생성을 위한 호출
	FResourceArrayInterface* ResourceArray;

	// 렌더 타겟에 클리어 색상을 바인딩하기 위한 값
	FClearValueBinding ClearValueBinding;

	// 리소스를 생성할 GPU 집합
	//FRHIGPUMask GPUMask;

	// 기본 리소스가 없는 RHI 객체 생성 여부
	bool bWithoutNativeResource;
	const TCHAR* DebugName;

	// 오프라인 쿠커 등에서 제공할 수 있는 선택적 데이터 - 범용
	uint32 ExtData;

	FName ClassName = NAME_None;    // Insight 자산 메타데이터 추적에 사용되는 FRHIBuffer의 소유자 클래스
	FName OwnerName = NAME_None;    // Insight 자산 메타데이터 추적에 사용되는 소유자 이름
};

struct FRHIBufferDesc
{
	uint32 Size{};
	uint32 Stride{};
	EBufferUsageFlags Usage{};

	FRHIBufferDesc() = default;
	FRHIBufferDesc(uint32 InSize, uint32 InStride, EBufferUsageFlags InUsage)
		: Size(InSize)
		, Stride(InStride)
		, Usage(InUsage)
	{
	}

	static FRHIBufferDesc Null()
	{
		return FRHIBufferDesc(0, 0, BUF_NullResource);
	}

	bool IsNull() const
	{
		if (EnumHasAnyFlags(Usage, BUF_NullResource))
		{
			// 널 리소스 디스크립터는 다른 필드가 0으로 설정되어야 하며, 추가 플래그가 없어야 합니다.
			_ASSERT(Size == 0 && Stride == 0 && Usage == BUF_NullResource);
			return true;
		}

		return false;
	}
};

class FRHIBuffer : public FRHIViewableResource
{
public:
	/** Initialization constructor. */
	FRHIBuffer(FRHIBufferDesc const& InDesc)
		: FRHIViewableResource(RRT_Buffer, ERHIAccess::Unknown) /* TODO (RemoveUnknowns): 리팩토링 후 디스크립터에서 InitialAccess를 사용합니다. */
		, Desc(InDesc)
	{
	}

	FRHIBufferDesc const& GetDesc() const { return Desc; }

	/** @return 버퍼의 바이트 수를 반환합니다. */
	uint32 GetSize() const { return Desc.Size; }

	/** @return 버퍼의 바이트 단위 스트라이드를 반환합니다. */
	uint32 GetStride() const { return Desc.Stride; }

	/** @return 버퍼를 생성할 때 사용된 사용 플래그를 반환합니다. */
	EBufferUsageFlags GetUsage() const { return Desc.Usage; }

	void SetName(const FName& InName) { Name = InName; }

	virtual uint32 GetParentGPUIndex() const { return 0; }

protected:
	// Used by RHI implementations that may adjust internal usage flags during object construction.
	void SetUsage(EBufferUsageFlags InUsage)
	{
		Desc.Usage = InUsage;
	}

	void TakeOwnership(FRHIBuffer& Other)
	{
		FRHIViewableResource::TakeOwnership(Other);
		Desc = Other.Desc;
	}

	void ReleaseOwnership()
	{
		FRHIViewableResource::ReleaseOwnership();
		Desc = FRHIBufferDesc::Null();
	}

private:
	FRHIBufferDesc Desc;
};

class FRHIUniformBuffer : public FRHIResource
{
public:
	FRHIUniformBuffer(const FConstantBufferInfo& InLayout) : FRHIResource(RRT_UniformBuffer), Layout(InLayout) {}

	uint32 GetBufferSize() const { return Layout.Size; }
	FConstantBufferInfo& GetLayout() { return Layout; }

protected:
	FConstantBufferInfo Layout;
};

//
// State blocks
//

class FRHISamplerState : public FRHIResource
{
public:
	FRHISamplerState() : FRHIResource(RRT_SamplerState) {}
	virtual bool IsImmutable() const { return false; }
	//virtual FRHIDescriptorHandle GetBindlessHandle() const { return FRHIDescriptorHandle(); }
};

class FRHIRasterizerState : public FRHIResource
{
public:
	FRHIRasterizerState() : FRHIResource(RRT_RasterizerState) {}
	virtual bool GetInitializer(struct FRasterizerStateInitializerRHI& Init) { return false; }
};

class FRHIDepthStencilState : public FRHIResource
{
public:
	FRHIDepthStencilState() : FRHIResource(RRT_DepthStencilState) {}

	virtual bool GetInitializer(struct FDepthStencilStateInitializerRHI& Init) { return false; }
};
