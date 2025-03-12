#pragma once
#include "DynamicRHI.h"
#include "RHIDefinitions.h"
#include "RHIGlobals.h"

/** Initializes the RHI. */
extern RHI_API void RHIInit();
extern RHI_API void RHIExit();

struct FVertexElement
{
	uint8 StreamIndex;
	uint8 Offset;
	EVertexElementType Type;
	uint8 AttributeIndex;
	uint16 Stride;
	///**
	// * 요소를 소비하기 위해 인스턴스 인덱스나 버텍스 인덱스를 사용할지 여부를 결정합니다.
	// * 예를 들어, bUseInstanceIndex가 0이면, 요소가 모든 인스턴스에 대해 반복됩니다.
	// */
	//uint16 bUseInstanceIndex;

	FVertexElement() {}
	FVertexElement(uint8 InStreamIndex, uint8 InOffset, EVertexElementType InType, uint8 InAttributeIndex, uint16 InStride/*, bool bInUseInstanceIndex = false*/) :
		StreamIndex(InStreamIndex),
		Offset(InOffset),
		Type(InType),
		AttributeIndex(InAttributeIndex),
		Stride(InStride)//,
		//bUseInstanceIndex(bInUseInstanceIndex)
	{
	}

	bool operator==(const FVertexElement& Other) const
	{
		return (StreamIndex == Other.StreamIndex &&
			Offset == Other.Offset &&
			Type == Other.Type &&
			AttributeIndex == Other.AttributeIndex &&
			Stride == Other.Stride);//&&
			//bUseInstanceIndex == Other.bUseInstanceIndex);
	}

	/*friend FArchive& operator<<(FArchive& Ar, FVertexElement& Element)
	{
		Ar << Element.StreamIndex;
		Ar << Element.Offset;
		Ar << Element.Type;
		Ar << Element.AttributeIndex;
		Ar << Element.Stride;
		Ar << Element.bUseInstanceIndex;
		return Ar;
	}*/
	//RHI_API FString ToString() const;
	//RHI_API void FromString(const FString& Src);
	//RHI_API void FromString(const FStringView& Src);
};


struct FSamplerStateInitializerRHI
{
	FSamplerStateInitializerRHI() {}
	FSamplerStateInitializerRHI(
		ESamplerFilter InFilter,
		ESamplerAddressMode InAddressU = AM_Wrap,
		ESamplerAddressMode InAddressV = AM_Wrap,
		ESamplerAddressMode InAddressW = AM_Wrap,
		float InMipBias = 0,
		int32 InMaxAnisotropy = 0,
		float InMinMipLevel = 0,
		float InMaxMipLevel = FLT_MAX,
		uint32 InBorderColor = 0,
		/** Only supported in D3D11 */
		ESamplerCompareFunction InSamplerComparisonFunction = SCF_Never
	)
		: Filter(InFilter)
		, AddressU(InAddressU)
		, AddressV(InAddressV)
		, AddressW(InAddressW)
		, MipBias(InMipBias)
		, MinMipLevel(InMinMipLevel)
		, MaxMipLevel(InMaxMipLevel)
		, MaxAnisotropy(InMaxAnisotropy)
		, BorderColor(InBorderColor)
		, SamplerComparisonFunction(InSamplerComparisonFunction)
	{
	}
	ESamplerFilter Filter = SF_Point;
	ESamplerAddressMode AddressU = AM_Wrap;
	ESamplerAddressMode AddressV = AM_Wrap;
	ESamplerAddressMode AddressW = AM_Wrap;
	float MipBias = 0.0f;
	/** Smallest mip map level that will be used, where 0 is the highest resolution mip level. */
	float MinMipLevel = 0.0f;
	/** Largest mip map level that will be used, where 0 is the highest resolution mip level. */
	float MaxMipLevel = FLT_MAX;
	int32 MaxAnisotropy = 0;
	uint32 BorderColor = 0;
	ESamplerCompareFunction SamplerComparisonFunction = SCF_Never;


	//RHI_API friend uint32 GetTypeHash(const FSamplerStateInitializerRHI& Initializer);
	//RHI_API friend bool operator== (const FSamplerStateInitializerRHI& A, const FSamplerStateInitializerRHI& B);
};

struct FRasterizerStateInitializerRHI
{
	ERasterizerFillMode FillMode = FM_Point;
	ERasterizerCullMode CullMode = CM_None;
	float DepthBias = 0.0f;
	float SlopeScaleDepthBias = 0.0f;
	ERasterizerDepthClipMode DepthClipMode = ERasterizerDepthClipMode::DepthClip;
	bool bAllowMSAA = false;

	FRasterizerStateInitializerRHI() = default;
	FRasterizerStateInitializerRHI(const FRasterizerStateInitializerRHI&) = default;
	FRasterizerStateInitializerRHI(FRasterizerStateInitializerRHI&&) = default;
	FRasterizerStateInitializerRHI& operator=(const FRasterizerStateInitializerRHI&) = default;

	FRasterizerStateInitializerRHI(ERasterizerFillMode InFillMode, ERasterizerCullMode InCullMode, bool bInAllowMSAA)
		: FillMode(InFillMode)
		, CullMode(InCullMode)
		, bAllowMSAA(bInAllowMSAA)
	{
	}

	FRasterizerStateInitializerRHI(ERasterizerFillMode InFillMode, ERasterizerCullMode InCullMode, float InDepthBias, float InSlopeScaleDepthBias, ERasterizerDepthClipMode InDepthClipMode, bool bInAllowMSAA)
		: FillMode(InFillMode)
		, CullMode(InCullMode)
		, DepthBias(InDepthBias)
		, SlopeScaleDepthBias(InSlopeScaleDepthBias)
		, DepthClipMode(InDepthClipMode)
		, bAllowMSAA(bInAllowMSAA)
	{
	}


	//RHI_API friend uint32 GetTypeHash(const FRasterizerStateInitializerRHI& Initializer);
	RHI_API friend bool operator== (const FRasterizerStateInitializerRHI& A, const FRasterizerStateInitializerRHI& B);
};

struct FDepthStencilStateInitializerRHI
{
	bool bEnableDepthWrite;
	ECompareFunction DepthTest;

	bool bEnableFrontFaceStencil;
	ECompareFunction FrontFaceStencilTest;
	EStencilOp FrontFaceStencilFailStencilOp;
	EStencilOp FrontFaceDepthFailStencilOp;
	EStencilOp FrontFacePassStencilOp;
	bool bEnableBackFaceStencil;
	ECompareFunction BackFaceStencilTest;
	EStencilOp BackFaceStencilFailStencilOp;
	EStencilOp BackFaceDepthFailStencilOp;
	EStencilOp BackFacePassStencilOp;
	uint8 StencilReadMask;
	uint8 StencilWriteMask;

	FDepthStencilStateInitializerRHI(
		bool bInEnableDepthWrite = true,
		ECompareFunction InDepthTest = CF_LessEqual,
		bool bInEnableFrontFaceStencil = false,
		ECompareFunction InFrontFaceStencilTest = CF_Always,
		EStencilOp InFrontFaceStencilFailStencilOp = SO_Keep,
		EStencilOp InFrontFaceDepthFailStencilOp = SO_Keep,
		EStencilOp InFrontFacePassStencilOp = SO_Keep,
		bool bInEnableBackFaceStencil = false,
		ECompareFunction InBackFaceStencilTest = CF_Always,
		EStencilOp InBackFaceStencilFailStencilOp = SO_Keep,
		EStencilOp InBackFaceDepthFailStencilOp = SO_Keep,
		EStencilOp InBackFacePassStencilOp = SO_Keep,
		uint8 InStencilReadMask = 0xFF,
		uint8 InStencilWriteMask = 0xFF
	)
		: bEnableDepthWrite(bInEnableDepthWrite)
		, DepthTest(InDepthTest)
		, bEnableFrontFaceStencil(bInEnableFrontFaceStencil)
		, FrontFaceStencilTest(InFrontFaceStencilTest)
		, FrontFaceStencilFailStencilOp(InFrontFaceStencilFailStencilOp)
		, FrontFaceDepthFailStencilOp(InFrontFaceDepthFailStencilOp)
		, FrontFacePassStencilOp(InFrontFacePassStencilOp)
		, bEnableBackFaceStencil(bInEnableBackFaceStencil)
		, BackFaceStencilTest(InBackFaceStencilTest)
		, BackFaceStencilFailStencilOp(InBackFaceStencilFailStencilOp)
		, BackFaceDepthFailStencilOp(InBackFaceDepthFailStencilOp)
		, BackFacePassStencilOp(InBackFacePassStencilOp)
		, StencilReadMask(InStencilReadMask)
		, StencilWriteMask(InStencilWriteMask)
	{
	}

	/*friend FArchive& operator<<(FArchive& Ar, FDepthStencilStateInitializerRHI& DepthStencilStateInitializer)
	{
		Ar << DepthStencilStateInitializer.bEnableDepthWrite;
		Ar << DepthStencilStateInitializer.DepthTest;
		Ar << DepthStencilStateInitializer.bEnableFrontFaceStencil;
		Ar << DepthStencilStateInitializer.FrontFaceStencilTest;
		Ar << DepthStencilStateInitializer.FrontFaceStencilFailStencilOp;
		Ar << DepthStencilStateInitializer.FrontFaceDepthFailStencilOp;
		Ar << DepthStencilStateInitializer.FrontFacePassStencilOp;
		Ar << DepthStencilStateInitializer.bEnableBackFaceStencil;
		Ar << DepthStencilStateInitializer.BackFaceStencilTest;
		Ar << DepthStencilStateInitializer.BackFaceStencilFailStencilOp;
		Ar << DepthStencilStateInitializer.BackFaceDepthFailStencilOp;
		Ar << DepthStencilStateInitializer.BackFacePassStencilOp;
		Ar << DepthStencilStateInitializer.StencilReadMask;
		Ar << DepthStencilStateInitializer.StencilWriteMask;
		return Ar;
	}*/

	//RHI_API friend uint32 GetTypeHash(const FDepthStencilStateInitializerRHI& Initializer);
	//RHI_API friend bool operator== (const FDepthStencilStateInitializerRHI& A, const FDepthStencilStateInitializerRHI& B);

	//RHI_API FString ToString() const;
	//RHI_API void FromString(const FString& Src);
	//RHI_API void FromString(const FStringView& Src);
};
