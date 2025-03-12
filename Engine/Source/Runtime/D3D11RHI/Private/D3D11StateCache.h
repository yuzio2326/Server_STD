#pragma once

class ID3D11DeviceContext;

//-----------------------------------------------------------------------------
//	FD3D11StateCache Class Definition
//-----------------------------------------------------------------------------
class FD3D11StateCache
{
public:
	void Init(ID3D11DeviceContext* InDeviceContext, bool bInAlwaysSetIndexBuffers = false)
	{
		SetContext(InDeviceContext);
	}

	virtual void SetContext(ID3D11DeviceContext* InDeviceContext)
	{
		Direct3DDeviceIMContext = InDeviceContext;
		ClearState();
	}

	/**
	 * 모든 D3D11 상태를 초기화하여 모든 입력/출력 리소스 슬롯, 셰이더, 입력 레이아웃,
	 * 예상 값, 가위 사각형, 깊이-스텐실 상태, 래스터라이저 상태, 블렌드 상태,
	 * 샘플러 상태 및 뷰포트를 NULL로 설정합니다.
	 */
	virtual void ClearState();

	inline void SetStreamStrides(const uint16* InStreamStrides)
	{
		memcpy(StreamStrides, InStreamStrides, sizeof(StreamStrides));
	}

	inline void SetInputLayout(ID3D11InputLayout* InputLayout)
	{
		if (CurrentInputLayout != InputLayout)
		{
			CurrentInputLayout = InputLayout;
			Direct3DDeviceIMContext->IASetInputLayout(InputLayout);
		}
	}

	inline void SetVertexShader(ID3D11VertexShader* Shader)
	{
		if (CurrentVertexShader != Shader)
		{
			CurrentVertexShader = Shader;
			Direct3DDeviceIMContext->VSSetShader(Shader, nullptr, 0);
		}
	}

	inline void SetPixelShader(ID3D11PixelShader* Shader)
	{
		if (CurrentPixelShader != Shader)
		{
			CurrentPixelShader = Shader;
			Direct3DDeviceIMContext->PSSetShader(Shader, nullptr, 0);
		}
	}

	inline void InternalSetStreamSource(ID3D11Buffer* VertexBuffer, uint32 StreamIndex, uint32 Stride, uint32 Offset)
	{
		_ASSERT(StreamIndex < ARRAYSIZE(CurrentVertexBuffers));
		FD3D11VertexBufferState& Slot = CurrentVertexBuffers[StreamIndex];
		if ((Slot.VertexBuffer != VertexBuffer || Slot.Offset != Offset || Slot.Stride != Stride) /*|| GD3D11SkipStateCaching*/)
		{
			Slot.VertexBuffer = VertexBuffer;
			Slot.Offset = Offset;
			Slot.Stride = Stride;
			Direct3DDeviceIMContext->IASetVertexBuffers(StreamIndex, 1, &VertexBuffer, &Stride, &Offset);
		}
	}

	inline void InternalSetIndexBuffer(ID3D11Buffer* IndexBuffer, DXGI_FORMAT Format, uint32 Offset)
	{
		if (/*bAlwaysSetIndexBuffers ||*/ (CurrentIndexBuffer != IndexBuffer || CurrentIndexFormat != Format || CurrentIndexOffset != Offset) /*|| GD3D11SkipStateCaching*/)
		{
			CurrentIndexBuffer = IndexBuffer;
			CurrentIndexFormat = Format;
			CurrentIndexOffset = Offset;
			Direct3DDeviceIMContext->IASetIndexBuffer(IndexBuffer, Format, Offset);
		}
	}

	inline void InternalSetSetConstantBuffer(EShaderFrequency ShaderFrequency, uint32 SlotIndex, ID3D11Buffer*& ConstantBuffer)
	{
		switch (ShaderFrequency)
		{
		case SF_Vertex:		Direct3DDeviceIMContext->VSSetConstantBuffers(SlotIndex, 1, &ConstantBuffer); break;
		case SF_Geometry:	Direct3DDeviceIMContext->GSSetConstantBuffers(SlotIndex, 1, &ConstantBuffer); break;
		case SF_Pixel:		Direct3DDeviceIMContext->PSSetConstantBuffers(SlotIndex, 1, &ConstantBuffer); break;
		case SF_Compute:	Direct3DDeviceIMContext->CSSetConstantBuffers(SlotIndex, 1, &ConstantBuffer); break;
		}
	}

	inline void SetConstantBuffer(EShaderFrequency ShaderFrequency, ID3D11Buffer* ConstantBuffer, uint32 SlotIndex)
	{
		ID3D11Buffer* Buffer = CurrentConstantBuffers[ShaderFrequency][SlotIndex];
		if (Buffer != ConstantBuffer)
		{
			CurrentConstantBuffers[ShaderFrequency][SlotIndex] = ConstantBuffer;
			InternalSetSetConstantBuffer(ShaderFrequency, SlotIndex, ConstantBuffer);
		}
	}

	inline void SetStreamSource(ID3D11Buffer* VertexBuffer, uint32 StreamIndex, uint32 Stride, uint32 Offset)
	{
		_ASSERT(Stride == StreamStrides[StreamIndex]);
		InternalSetStreamSource(VertexBuffer, StreamIndex, Stride, Offset);
	}

	inline void SetStreamSource(ID3D11Buffer* VertexBuffer, uint32 StreamIndex, uint32 Offset)
	{
		InternalSetStreamSource(VertexBuffer, StreamIndex, StreamStrides[StreamIndex], Offset);
	}

	inline void SetIndexBuffer(ID3D11Buffer* IndexBuffer, DXGI_FORMAT Format, uint32 Offset)
	{
		InternalSetIndexBuffer(IndexBuffer, Format, Offset);
	}

	inline void SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY PrimitiveTopology)
	{
		if (CurrentPrimitiveTopology != PrimitiveTopology)
		{
			CurrentPrimitiveTopology = PrimitiveTopology;
			Direct3DDeviceIMContext->IASetPrimitiveTopology(PrimitiveTopology);
		}
	}

	template <EShaderFrequency ShaderFrequency>
	inline void ClearConstantBuffers()
	{
		ZeroMemory(CurrentConstantBuffers[ShaderFrequency], sizeof(CurrentConstantBuffers[ShaderFrequency]));
		ID3D11Buffer* Empty[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT] = { 0 };
		switch (ShaderFrequency)
		{
		case SF_Vertex:		Direct3DDeviceIMContext->VSSetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, Empty); break;
		case SF_Geometry:	Direct3DDeviceIMContext->GSSetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, Empty); break;
		case SF_Pixel:		Direct3DDeviceIMContext->PSSetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, Empty); break;
		case SF_Compute:	Direct3DDeviceIMContext->CSSetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, Empty); break;
		}
	}

	inline void SetRasterizerState(ID3D11RasterizerState* State)
	{
		if (CurrentRasterizerState != State)
		{
			CurrentRasterizerState = State;
			Direct3DDeviceIMContext->RSSetState(State);
		}
	}

	inline void SetViewport(D3D11_VIEWPORT Viewport)
	{
		if (CurrentNumberOfViewports != 1 || memcmp(&CurrentViewport[0], &Viewport, sizeof(D3D11_VIEWPORT)))
		{
			memcpy(&CurrentViewport[0], &Viewport, sizeof(D3D11_VIEWPORT));
			CurrentNumberOfViewports = 1;
			Direct3DDeviceIMContext->RSSetViewports(1, &Viewport);
		}
	}

	inline void GetViewport(D3D11_VIEWPORT* Viewport)
	{
		_ASSERT(Viewport);
		memcpy(Viewport, &CurrentViewport, sizeof(D3D11_VIEWPORT));
	}

	inline void SetDepthStencilState(ID3D11DepthStencilState* State, uint32 RefStencil)
	{
		if (CurrentDepthStencilState != State || CurrentReferenceStencil != RefStencil)
		{
			CurrentDepthStencilState = State;
			CurrentReferenceStencil = RefStencil;
			Direct3DDeviceIMContext->OMSetDepthStencilState(State, RefStencil);
		}
	}

	template <EShaderFrequency ShaderFrequency>
	inline void InternalSetSamplerState(uint32 SamplerIndex, ID3D11SamplerState*& SamplerState)
	{
		//CA_SUPPRESS(6326);
		switch (ShaderFrequency)
		{
		case SF_Vertex:		Direct3DDeviceIMContext->VSSetSamplers(SamplerIndex, 1, &SamplerState); break;
		case SF_Geometry:	Direct3DDeviceIMContext->GSSetSamplers(SamplerIndex, 1, &SamplerState); break;
		case SF_Pixel:		Direct3DDeviceIMContext->PSSetSamplers(SamplerIndex, 1, &SamplerState); break;
		case SF_Compute:	Direct3DDeviceIMContext->CSSetSamplers(SamplerIndex, 1, &SamplerState); break;
		}
	}

	template <EShaderFrequency ShaderFrequency>
	inline void SetSamplerState(ID3D11SamplerState* SamplerState, uint32 SamplerIndex)
	{
		InternalSetSamplerState<ShaderFrequency>(SamplerIndex, SamplerState);
	}

	template <EShaderFrequency ShaderFrequency>
	inline void InternalSetShaderResourceView(uint32 ResourceIndex, ID3D11ShaderResourceView*& SRV)
	{
		// Set the SRV we have been given (or null).
		//CA_SUPPRESS(6326);
		switch (ShaderFrequency)
		{
		case SF_Vertex:		Direct3DDeviceIMContext->VSSetShaderResources(ResourceIndex, 1, &SRV); break;
		case SF_Geometry:	Direct3DDeviceIMContext->GSSetShaderResources(ResourceIndex, 1, &SRV); break;
		case SF_Pixel:		Direct3DDeviceIMContext->PSSetShaderResources(ResourceIndex, 1, &SRV); break;
		case SF_Compute:	Direct3DDeviceIMContext->CSSetShaderResources(ResourceIndex, 1, &SRV); break;
		}
	}


	template <EShaderFrequency ShaderFrequency>
	inline void InternalSetShaderResourceView(ID3D11ShaderResourceView*& SRV, uint32 ResourceIndex)
	{
		_ASSERT(ResourceIndex < ARRAYSIZE(CurrentShaderResourceViews[ShaderFrequency]));
		if (CurrentShaderResourceViews[ShaderFrequency][ResourceIndex] != SRV)
		{
			if (SRV)
			{
				SRV->AddRef();
			}
			if (CurrentShaderResourceViews[ShaderFrequency][ResourceIndex])
			{
				CurrentShaderResourceViews[ShaderFrequency][ResourceIndex]->Release();
			}
			CurrentShaderResourceViews[ShaderFrequency][ResourceIndex] = SRV;
			InternalSetShaderResourceView<ShaderFrequency>(ResourceIndex, SRV);
		}
	}

	template <EShaderFrequency ShaderFrequency>
	inline void SetShaderResourceView(ID3D11ShaderResourceView* SRV, uint32 ResourceIndex)
	{
		InternalSetShaderResourceView<ShaderFrequency>(SRV, ResourceIndex);
	}

protected:
	ID3D11DeviceContext* Direct3DDeviceIMContext = nullptr;
	
	// Shader Resource Views Cache
	ID3D11ShaderResourceView* CurrentShaderResourceViews[SF_NumStandardFrequencies][D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {};

	// Input Layout State
	ID3D11InputLayout* CurrentInputLayout = nullptr;

	// Rasterizer State Cache
	ID3D11RasterizerState* CurrentRasterizerState = nullptr;

	// Depth Stencil State Cache
	uint32 CurrentReferenceStencil = 0;
	ID3D11DepthStencilState* CurrentDepthStencilState = nullptr;

	// Shader Cache
	ID3D11VertexShader* CurrentVertexShader = nullptr;
	//ID3D11GeometryShader* CurrentGeometryShader;
	ID3D11PixelShader* CurrentPixelShader = nullptr;
	//ID3D11ComputeShader* CurrentComputeShader;

	// Vertex Buffer State
	struct FD3D11VertexBufferState
	{
		ID3D11Buffer* VertexBuffer;
		uint32 Stride;
		uint32 Offset;
	} CurrentVertexBuffers[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = {};

	// Index Buffer State
	ID3D11Buffer* CurrentIndexBuffer = nullptr;
	DXGI_FORMAT CurrentIndexFormat = {};
	uint32 CurrentIndexOffset = 0;

	// Primitive Topology State
	D3D11_PRIMITIVE_TOPOLOGY CurrentPrimitiveTopology = {};

	uint16 StreamStrides[MaxVertexElementCount] = {};

	// Viewport
	uint32			CurrentNumberOfViewports = 0;
	D3D11_VIEWPORT CurrentViewport[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE] = {};

	ID3D11Buffer* CurrentConstantBuffers[EShaderFrequency::SF_NumStandardFrequencies][D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT] = {};
};