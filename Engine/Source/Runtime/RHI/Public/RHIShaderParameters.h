#pragma once

#include "RHIResources.h"

class FRHICommandList;

/** 바인딩된 셰이더 매개변수의 간단한 표현 (읽기: 값). 오프셋은 연결된 blob에서 데이터를 참조하기 위한 것입니다. */
struct FRHIShaderParameter
{
    FRHIShaderParameter(uint16 InBufferIndex, uint16 InBaseIndex, uint16 InByteOffset, uint16 InByteSize)
        : BufferIndex(InBufferIndex)
        , BaseIndex(InBaseIndex)
        , ByteOffset(InByteOffset)
        , ByteSize(InByteSize)
    {
    }
    uint16 BufferIndex;
    uint16 BaseIndex;
    uint16 ByteOffset;
    uint16 ByteSize;
};

/** 바인딩된 리소스 파라미터의 간단한 표현 (텍스처, SRV, UAV, 샘플러 상태, 또는 유니폼 버퍼) */
struct FRHIShaderParameterResource
{
    enum class EType : uint8
    {
        Texture,
        ResourceView,
        UnorderedAccessView,
        Sampler,
        UniformBuffer,
    };

    FRHIShaderParameterResource() = default;
    FRHIShaderParameterResource(EType InType, FRHIResource* InResource, uint16 InIndex)
        : Resource(InResource)
        , Index(InIndex)
        , Type(InType)
    {
    }
    FRHIShaderParameterResource(FRHITexture* InTexture, uint16 InIndex)
        : FRHIShaderParameterResource(FRHIShaderParameterResource::EType::Texture, InTexture, InIndex)
    {
    }
    /*FRHIShaderParameterResource(FRHIShaderResourceView* InView, uint16 InIndex)
        : FRHIShaderParameterResource(FRHIShaderParameterResource::EType::ResourceView, InView, InIndex)
    {
    }*/
    /*FRHIShaderParameterResource(FRHIUnorderedAccessView* InUAV, uint16 InIndex)
        : FRHIShaderParameterResource(FRHIShaderParameterResource::EType::UnorderedAccessView, InUAV, InIndex)
    {
    }*/
    FRHIShaderParameterResource(FRHISamplerState* InSamplerState, uint16 InIndex)
        : FRHIShaderParameterResource(FRHIShaderParameterResource::EType::Sampler, InSamplerState, InIndex)
    {
    }
    /*FRHIShaderParameterResource(FRHIUniformBuffer* InUniformBuffer, uint16 InIndex)
        : FRHIShaderParameterResource(FRHIShaderParameterResource::EType::UniformBuffer, InUniformBuffer, InIndex)
    {
    }*/

    FRHIResource* Resource = nullptr;
    uint16        Index = 0;
    EType         Type = EType::Texture;
};
