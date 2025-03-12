
// DeviceContext 상태 캐싱 구현을 통해 불필요한 기기 컨텍스트 호출을 제거하여
// 렌더링 스레드 성능을 개선합니다.

//-----------------------------------------------------------------------------
//	Include Files
//-----------------------------------------------------------------------------
#include "D3D11RHIPrivate.h"
#include "D3D11StateCache.h"

void FD3D11StateCache::ClearState()
{
	if (Direct3DDeviceIMContext)
	{
		Direct3DDeviceIMContext->ClearState();
	}

	// Shader Resource View State Cache
	for (uint32 ShaderFrequency = 0; ShaderFrequency < SF_NumStandardFrequencies; ShaderFrequency++)
	{
		for (uint32 Index = 0; Index < D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT; Index++)
		{
			if (CurrentShaderResourceViews[ShaderFrequency][Index])
			{
				CurrentShaderResourceViews[ShaderFrequency][Index]->Release();
				CurrentShaderResourceViews[ShaderFrequency][Index] = NULL;
			}
		}
	}

	// Rasterizer State Cache
	CurrentRasterizerState = nullptr;

	//// Depth Stencil State Cache
	CurrentReferenceStencil = 0;
	CurrentDepthStencilState = nullptr;
	//bDepthBoundsEnabled = false;
	//DepthBoundsMin = 0.0f;
	//DepthBoundsMax = 1.0f;
	
	ZeroMemory(StreamStrides, sizeof(StreamStrides));

	// Shader Cache
	CurrentVertexShader = nullptr;
	//CurrentGeometryShader = nullptr;
	CurrentPixelShader = nullptr;
	//CurrentComputeShader = nullptr;

	// Blend State Cache
	//CurrentBlendFactor[0] = 1.0f;
	//CurrentBlendFactor[1] = 1.0f;
	//CurrentBlendFactor[2] = 1.0f;
	//CurrentBlendFactor[3] = 1.0f;

	ZeroMemory(&CurrentViewport[0], 0, sizeof(D3D11_VIEWPORT) * D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE);
	CurrentNumberOfViewports = 0;

	//CurrentBlendSampleMask = 0xffffffff;
	//CurrentBlendState = nullptr;

	CurrentInputLayout = nullptr;

	ZeroMemory(CurrentVertexBuffers, sizeof(CurrentVertexBuffers));
	//FMemory::Memzero(CurrentSamplerStates, sizeof(CurrentSamplerStates));

	CurrentIndexBuffer = nullptr;
	CurrentIndexFormat = DXGI_FORMAT_UNKNOWN;

	CurrentIndexOffset = 0;
	CurrentPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;

	for (uint32 Frequency = 0; Frequency < SF_NumStandardFrequencies; Frequency++)
	{
		for (uint32 Index = 0; Index < D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT; Index++)
		{
			CurrentConstantBuffers[Frequency][Index] = nullptr;
		}
	}
}
