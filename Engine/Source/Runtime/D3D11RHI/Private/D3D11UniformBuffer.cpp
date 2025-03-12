#include "D3D11RHIPrivate.h"

unordered_map<FString, FUniformBufferRHIRef> UniformBuffers;

FUniformBufferRHIRef FD3D11DynamicRHI::RHICreateUniformBuffer(const FConstantBufferInfo& Layout, const void* Contents, const uint32 ContentsSize)
{
	if (UniformBuffers.contains(Layout.Name))
	{
		if (Layout.Size != ContentsSize)
		{
			E_LOG(Error, TEXT("Size 가 다릅니다"));
		}

		FRHIUniformBuffer* UniformBuffer = UniformBuffers[Layout.Name];

		if (Contents)
		{
			RHIUpdateUniformBuffer(UniformBuffer, Contents, ContentsSize);
		}

		return UniformBuffer;
	}

	FD3D11UniformBuffer* NewUniformBuffer = nullptr;
	const uint32 NumBytes = Layout.Size;
	if (Layout.Size != ContentsSize)
	{
		E_LOG(Error, TEXT("{} Layout.Size({}) != ContentsSize({})"), Layout.Name, Layout.Size, ContentsSize);
		return nullptr;
	}

	D3D11_BUFFER_DESC Desc;
	Desc.ByteWidth = NumBytes;
	Desc.Usage = D3D11_USAGE_DYNAMIC;
	Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	Desc.MiscFlags = 0;
	Desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA ImmutableData;
	ImmutableData.pSysMem = Contents;
	ImmutableData.SysMemPitch = ImmutableData.SysMemSlicePitch = 0;

	TRefCountPtr<ID3D11Buffer> UniformBufferResource;
	VERIFYD3D11RESULT_EX(Direct3DDevice->CreateBuffer(&Desc, Contents ? &ImmutableData : nullptr, UniformBufferResource.GetInitReference()), Direct3DDevice);

	NewUniformBuffer = new FD3D11UniformBuffer(Layout, UniformBufferResource);

	UniformBuffers.emplace(Layout.Name, NewUniformBuffer);

	return NewUniformBuffer;
}

void FD3D11DynamicRHI::RHIUpdateUniformBuffer(FRHIUniformBuffer* UniformBufferRHI, const void* Contents, const uint32 ContentsSize)
{
	FD3D11UniformBuffer* UniformBuffer = ResourceCast(UniformBufferRHI);
	const uint32 ConstantBufferSize = UniformBuffer->GetBufferSize();
	// 유니폼 버퍼의 내용을 업데이트합니다.
	if (ConstantBufferSize > 0)
	{
		// 상수 버퍼는 16바이트로 정렬되어야 합니다.
		//_ASSERT(Align(Contents, 16) == Contents);

		D3D11_MAPPED_SUBRESOURCE MappedSubresource;
		// 항상 전체 업데이트를 수행하기 때문에 이전 결과를 폐기합니다.
		VERIFYD3D11RESULT_EX(Direct3DDeviceIMContext->Map(UniformBuffer->Resource.GetReference(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource), Direct3DDevice);
		_ASSERT(MappedSubresource.RowPitch >= ConstantBufferSize);
		memcpy(MappedSubresource.pData, Contents, ConstantBufferSize);
		Direct3DDeviceIMContext->Unmap(UniformBuffer->Resource.GetReference(), 0);
	}
}

void FD3D11DynamicRHI::RHISetShaderUniformBuffer(EShaderFrequency Frequency, uint8 RegisterIndex, FRHIUniformBuffer* UniformBufferRHI)
{
	FD3D11UniformBuffer* UniformBuffer = ResourceCast(UniformBufferRHI);

	StateCache.SetConstantBuffer(Frequency, UniformBuffer->Resource, RegisterIndex);
}