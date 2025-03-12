#pragma once
#include "RHIResources.h"
#include "D3D11ThirdParty.h"
#include "D3D11State.h"

/** 셰이더 리소스로 바인딩될 수 있는 리소스의 기본 클래스. */
class FD3D11ViewableResource
{
public:
	~FD3D11ViewableResource()
	{
		_ASSERT(!HasLinkedViews(), TEXT("All linked views must have been removed before the underlying resource can be deleted."));
	}

	bool HasLinkedViews() const
	{
		return LinkedViews != nullptr;
	}

	void UpdateLinkedViews();

private:
	friend class FD3D11ShaderResourceView;
	friend class FD3D11UnorderedAccessView;
	class FD3D11View* LinkedViews = nullptr;
};

/** Texture base class. */
class FD3D11Texture final : public FRHITexture, public FD3D11ViewableResource
{
public:
	D3D11RHI_API explicit FD3D11Texture(
		const FRHITextureCreateDesc& InDesc,
		ID3D11Resource* InResource,
		ID3D11ShaderResourceView* InShaderResourceView,
		int32 InRTVArraySize,
		bool bInCreatedRTVsPerSlice,
		TArray<TRefCountPtr<ID3D11RenderTargetView>> InRenderTargetViews,
		TArray<TRefCountPtr<ID3D11DepthStencilView>> InDepthStencilViews
	);

	enum EAliasResourceParam { CreateAlias };
	D3D11RHI_API explicit FD3D11Texture(FD3D11Texture const& Other, const FString& Name, EAliasResourceParam);
	D3D11RHI_API void AliasResource(FD3D11Texture const& Other);

	D3D11RHI_API virtual ~FD3D11Texture();

	/*inline uint64 GetMemorySize() const
	{
		return RHICalcTexturePlatformSize(GetDesc()).Size;
	}*/

	// Accessors.
	inline ID3D11Resource* GetResource() const { return Resource; }
	inline ID3D11ShaderResourceView* GetShaderResourceView() const { return ShaderResourceView; }

	inline bool IsCubemap() const
	{
		FRHITextureDesc const& Desc = GetDesc();
		return Desc.Dimension == ETextureDimension::TextureCube || Desc.Dimension == ETextureDimension::TextureCubeArray;
	}

	inline ID3D11Texture2D* GetD3D11Texture2D() const
	{
		_ASSERT(Resource);
		_ASSERT(GetDesc().Dimension == ETextureDimension::Texture2D
			|| GetDesc().Dimension == ETextureDimension::Texture2DArray
			|| GetDesc().Dimension == ETextureDimension::TextureCube
			|| GetDesc().Dimension == ETextureDimension::TextureCubeArray);

		return static_cast<ID3D11Texture2D*>(Resource.GetReference());
	}

	inline ID3D11Texture3D* GetD3D11Texture3D() const
	{
		_ASSERT(Resource);
		_ASSERT(GetDesc().Dimension == ETextureDimension::Texture3D);

		return static_cast<ID3D11Texture3D*>(Resource.GetReference());
	}

	inline bool IsTexture3D() const
	{
		return GetDesc().Dimension == ETextureDimension::Texture3D;
	}

	virtual inline void* GetNativeResource() const override
	{
		return GetResource();
	}

	virtual inline void* GetNativeShaderResourceView() const override
	{
		return GetShaderResourceView();
	}

	virtual inline void* GetTextureBaseRHI() override
	{
		return this;
	}

	inline void SetIHVResourceHandle(void* InHandle)
	{
		IHVResourceHandle = InHandle;
	}

	inline void* GetIHVResourceHandle() const
	{
		return IHVResourceHandle;
	}

	/**
	 * Get the render target view for the specified mip and array slice.
	 * An array slice of -1 is used to indicate that no array slice should be required.
	 */
	inline ID3D11RenderTargetView* GetRenderTargetView(int32 MipIndex, int32 ArraySliceIndex) const
	{
		int32 ArrayIndex = MipIndex;

		if (bCreatedRTVsPerSlice)
		{
			_ASSERT(ArraySliceIndex >= 0);
			ArrayIndex = MipIndex * RTVArraySize + ArraySliceIndex;
		}
		else
		{
			// Catch attempts to use a specific slice without having created the texture to support it
			_ASSERT(ArraySliceIndex == -1 || ArraySliceIndex == 0);
		}

		if ((uint32)ArrayIndex < (uint32)RenderTargetViews.size())
		{
			return RenderTargetViews[ArrayIndex];
		}
		return 0;
	}

	inline ID3D11DepthStencilView* GetDepthStencilView(FExclusiveDepthStencil AccessType) const
	{
		return DepthStencilViews[AccessType.GetIndex()];
	}

//#if RHI_ENABLE_RESOURCE_INFO
//	virtual bool GetResourceInfo(FRHIResourceInfo& OutResourceInfo) const override
//	{
//		OutResourceInfo = FRHIResourceInfo{};
//		OutResourceInfo.Name = GetName();
//		OutResourceInfo.Type = GetType();
//		OutResourceInfo.VRamAllocation.AllocationSize = GetMemorySize();
//		return true;
//	}
//#endif

	/**
	* Locks one of the texture's mip-maps.
	* @return A pointer to the specified texture data.
	*/
	//D3D11RHI_API void* Lock(class FD3D11DynamicRHI* D3DRHI, uint32 MipIndex, uint32 ArrayIndex, EResourceLockMode LockMode, uint32& DestStride, bool bForceLockDeferred = false, uint64* OutLockedByteCount = nullptr);
	//
	///** Unlocks a previously locked mip-map. */
	//D3D11RHI_API void Unlock(class FD3D11DynamicRHI* D3DRHI, uint32 MipIndex, uint32 ArrayIndex);

private:
	//Resource handle for use by IHVs for SLI and other purposes.
	void* IHVResourceHandle = nullptr;

	/** The texture resource. */
	TRefCountPtr<ID3D11Resource> Resource;

	/** A shader resource view of the texture. */
	TRefCountPtr<ID3D11ShaderResourceView> ShaderResourceView;

	/** A render targetable view of the texture. */
	TArray<TRefCountPtr<ID3D11RenderTargetView> > RenderTargetViews;

	/** A depth-stencil targetable view of the texture. */
	TRefCountPtr<ID3D11DepthStencilView> DepthStencilViews[FExclusiveDepthStencil::MaxIndex];

	int32 RTVArraySize;

	uint8 bCreatedRTVsPerSlice : 1;
	uint8 bAlias : 1;
};

/**
 * 특정 선언과 결합되지 않아 바인딩되지 않은 셰이더를 나타내는 버텍스 셰이더입니다.
 */
class FD3D11VertexShader : public FRHIVertexShader
{
public:
	enum { StaticFrequency = SF_Vertex };

	/**
	 * 버텍스 셰이더 리소스.
	 */
	TRefCountPtr<ID3D11VertexShader> Resource;

	/**
	 * 사용자 정의 데이터가 첨부된 버텍스 셰이더의 바이트코드.
	 */
	TArray<uint8> Code;
};

class FD3D11PixelShader : public FRHIPixelShader
{
public:
	enum { StaticFrequency = SF_Pixel };

	/** The shader resource. */
	TRefCountPtr<ID3D11PixelShader> Resource;
};

/**
 * 편의 typedef: 사전 할당된 D3D11 입력 요소 설명 배열.
 */
typedef TArray<D3D11_INPUT_ELEMENT_DESC> FD3D11VertexElements;

/**
 * 특정 셰이더와 결합되지 않은 상태의 버텍스 선언을 나타냅니다.
 */
class FD3D11VertexDeclaration : public FRHIVertexDeclaration
{
public:
	/** 버텍스 선언의 요소들. */
	FD3D11VertexElements VertexElements;

	uint16 StreamStrides[MaxVertexElementCount];

	/** 초기화 생성자. */
	explicit FD3D11VertexDeclaration(const FD3D11VertexElements& InElements, const uint16* InStrides)
		: VertexElements(InElements)
	{
		memcpy(StreamStrides, InStrides, sizeof(StreamStrides));
	}
};

/**
 * 지오메트리를 렌더링하기 위한 결합된 셰이더 상태 및 버텍스 정의.
 * 각 고유 인스턴스는 버텍스 선언, 버텍스 셰이더 및 픽셀 셰이더로 구성됩니다.
 */
class FD3D11BoundShaderState : public FRHIBoundShaderState
{
public:
	uint16 StreamStrides[MaxVertexElementCount] = {};
	TRefCountPtr<ID3D11InputLayout> InputLayout;
	TRefCountPtr<ID3D11VertexShader> VertexShader;
	TRefCountPtr<ID3D11PixelShader> PixelShader;

	/** Initialization constructor. */
	FD3D11BoundShaderState(
		FRHIVertexDeclaration* InVertexDeclarationRHI,
		FRHIVertexShader* InVertexShaderRHI,
		FRHIPixelShader* InPixelShaderRHI,
		//FRHIGeometryShader* InGeometryShaderRHI,
		ID3D11Device* Direct3DDevice
	);
};

/** 버퍼 리소스 클래스. */
class FD3D11Buffer : public FRHIBuffer, public FD3D11ViewableResource
{
public:

	TRefCountPtr<ID3D11Buffer> Resource;

	FD3D11Buffer(ID3D11Buffer* InResource, FRHIBufferDesc const& InDesc)
		: FRHIBuffer(InDesc)
		, Resource(InResource)
	{
	}

	// FRHIResource 오버라이드
//#if RHI_ENABLE_RESOURCE_INFO
//	bool GetResourceInfo(FRHIResourceInfo& OutResourceInfo) const override
//	{
//		OutResourceInfo = FRHIResourceInfo{};
//		OutResourceInfo.Name = GetName();
//		OutResourceInfo.Type = GetType();
//		OutResourceInfo.VRamAllocation.AllocationSize = GetSize();
//		return true;
//	}
//#endif

	virtual ~FD3D11Buffer() {}

	//void TakeOwnership(FD3D11Buffer& Other);
	//void ReleaseOwnership();

	// IRefCountedObject 인터페이스
	virtual uint32 AddRef() const
	{
		return FRHIResource::AddRef();
	}
	virtual uint32 Release() const
	{
		return FRHIResource::Release();
	}
	virtual uint32 GetRefCount() const
	{
		return FRHIResource::GetRefCount();
	}
};

/** Uniform buffer resource class. */
class FD3D11UniformBuffer : public FRHIUniformBuffer
{
public:
	/** The D3D11 constant buffer resource */
	TRefCountPtr<ID3D11Buffer> Resource;

	FD3D11UniformBuffer() = delete;
	FD3D11UniformBuffer(const FConstantBufferInfo& InLayout, TRefCountPtr<ID3D11Buffer> InResource)
		: FRHIUniformBuffer(InLayout), Resource(InResource) { }
};

template<class T>
struct TD3D11ResourceTraits
{
};
template<>
struct TD3D11ResourceTraits<FRHIVertexDeclaration>
{
	typedef FD3D11VertexDeclaration TConcreteType;
};
template<>
struct TD3D11ResourceTraits<FRHIVertexShader>
{
	typedef FD3D11VertexShader TConcreteType;
};
//template<>
//struct TD3D11ResourceTraits<FRHIGeometryShader>
//{
//	typedef FD3D11GeometryShader TConcreteType;
//};
template<>
struct TD3D11ResourceTraits<FRHIPixelShader>
{
	typedef FD3D11PixelShader TConcreteType;
};
//template<>
//struct TD3D11ResourceTraits<FRHIComputeShader>
//{
//	typedef FD3D11ComputeShader TConcreteType;
//};
template<>
struct TD3D11ResourceTraits<FRHIBoundShaderState>
{
	typedef FD3D11BoundShaderState TConcreteType;
};
//template<>
//struct TD3D11ResourceTraits<FRHIRenderQuery>
//{
//	typedef FD3D11RenderQuery TConcreteType;
//};
template<>
struct TD3D11ResourceTraits<FRHIUniformBuffer>
{
	typedef FD3D11UniformBuffer TConcreteType;
};
template<>
struct TD3D11ResourceTraits<FRHIBuffer>
{
	typedef FD3D11Buffer TConcreteType;
};
//template<>
//struct TD3D11ResourceTraits<FRHIStagingBuffer>
//{
//	typedef FD3D11StagingBuffer TConcreteType;
//};
//// @todo-staging Implement D3D11 fences.
//template<>
//struct TD3D11ResourceTraits<FRHIGPUFence>
//{
//	typedef FGenericRHIGPUFence TConcreteType;
//};
//template<>
//struct TD3D11ResourceTraits<FRHIShaderResourceView>
//{
//	typedef FD3D11ShaderResourceView TConcreteType;
//};
//template<>
//struct TD3D11ResourceTraits<FRHIUnorderedAccessView>
//{
//	typedef FD3D11UnorderedAccessView TConcreteType;
//};

template<>
struct TD3D11ResourceTraits<FRHISamplerState>
{
	typedef FD3D11SamplerState TConcreteType;
};
template<>
struct TD3D11ResourceTraits<FRHIRasterizerState>
{
	typedef FD3D11RasterizerState TConcreteType;
};
template<>
struct TD3D11ResourceTraits<FRHIDepthStencilState>
{
	typedef FD3D11DepthStencilState TConcreteType;
};
//template<>
//struct TD3D11ResourceTraits<FRHIBlendState>
//{
//	typedef FD3D11BlendState TConcreteType;
//};
