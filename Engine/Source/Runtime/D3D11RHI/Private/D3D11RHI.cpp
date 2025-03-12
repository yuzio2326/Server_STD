#include "D3D11RHIPrivate.h"

void FD3D11DynamicRHI::ClearState()
{
    StateCache.ClearState();
}


template <EShaderFrequency ShaderFrequency>
void FD3D11DynamicRHI::ClearAllShaderResourcesForFrequency()
{
    int32 MaxIndex = MaxBoundShaderResourcesIndex[ShaderFrequency];
    for (int32 ResourceIndex = MaxIndex; ResourceIndex >= 0; --ResourceIndex)
    {
        if (CurrentResourcesBoundAsSRVs[ShaderFrequency][ResourceIndex] != nullptr)
        {
            // Unset the SRV from the device context
            SetShaderResourceView<ShaderFrequency>(nullptr, nullptr, ResourceIndex);
        }
    }
    StateCache.ClearConstantBuffers<ShaderFrequency>();
}

void FD3D11DynamicRHI::ClearAllShaderResources()
{
    ClearAllShaderResourcesForFrequency<SF_Vertex>();
    //ClearAllShaderResourcesForFrequency<SF_Geometry>();
    ClearAllShaderResourcesForFrequency<SF_Pixel>();
    //ClearAllShaderResourcesForFrequency<SF_Compute>();
}

void FD3D11DynamicRHI::TrackResourceBoundAsVB(FD3D11ViewableResource* Resource, int32 StreamIndex)
{
    _ASSERT(StreamIndex >= 0 && StreamIndex < D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT);
    if (Resource)
    {
        // 새로운 버텍스 버퍼(VB)를 바인딩합니다.
        // 가장 높은 바인딩된 리소스 인덱스로 최대 리소스 인덱스를 업데이트합니다.
        MaxBoundVertexBufferIndex = FMath::Max(MaxBoundVertexBufferIndex, StreamIndex);
        CurrentResourcesBoundAsVBs[StreamIndex] = Resource;
    }
    else if (CurrentResourcesBoundAsVBs[StreamIndex] != nullptr)
    {
        // 해당 슬롯에서 리소스를 언바인딩합니다.
        CurrentResourcesBoundAsVBs[StreamIndex] = nullptr;

        // 만약 이것이 가장 높은 바인딩된 리소스였다면...
        if (MaxBoundVertexBufferIndex == StreamIndex)
        {
            // 다음 non-null 슬롯을 찾거나 슬롯이 없을 때까지
            // 최대 리소스 인덱스를 하향 조정합니다.
            do
            {
                MaxBoundVertexBufferIndex--;
            } while (MaxBoundVertexBufferIndex >= 0 && CurrentResourcesBoundAsVBs[MaxBoundVertexBufferIndex] == nullptr);
        }
    }
}


void FD3D11DynamicRHI::TrackResourceBoundAsIB(FD3D11ViewableResource* Resource)
{
	CurrentResourceBoundAsIB = Resource;
}

template<EShaderFrequency ShaderFrequency>
void FD3D11DynamicRHI::InternalSetShaderResourceView(FD3D11ViewableResource* Resource, ID3D11ShaderResourceView* SRV, int32 ResourceIndex)
{
    // 둘 다 설정되었거나 둘 다 null인지 확인합니다.
    _ASSERT((Resource && SRV) || (!Resource && !SRV));

    // 상태 캐시 충돌 방지
    if (!((Resource && SRV) || (!Resource && !SRV)))
    {
        //UE_LOG(LogRHI, Warning, TEXT("리소스: %i, %s에서 InternalSetShaderResourceView를 중단합니다"), ResourceIndex, *SRVName.ToString());
        return;
    }

    FD3D11ViewableResource*& ResourceSlot = CurrentResourcesBoundAsSRVs[ShaderFrequency][ResourceIndex];
    int32& MaxResourceIndex = MaxBoundShaderResourcesIndex[ShaderFrequency];

    if (Resource)
    {
        // 새로운 SRV를 바인딩합니다.
        // 최대 리소스 인덱스를 가장 높은 바인딩된 리소스 인덱스로 업데이트합니다.
        MaxResourceIndex = FMath::Max(MaxResourceIndex, ResourceIndex);
        ResourceSlot = Resource;
    }
    else if (ResourceSlot != nullptr)
    {
        // 슬롯에서 리소스를 언바인딩합니다.
        ResourceSlot = nullptr;

        // 만약 이것이 가장 높은 바인딩된 리소스라면...
        if (MaxResourceIndex == ResourceIndex)
        {
            // 다음 비-null 슬롯을 찾거나 슬롯이 모두 비어있을 때까지
            // 최대 리소스 인덱스를 줄입니다.
            do
            {
                MaxResourceIndex--;
            } while (MaxResourceIndex >= 0 && CurrentResourcesBoundAsSRVs[ShaderFrequency][MaxResourceIndex] == nullptr);
        }
    }

    // 주어진 SRV(또는 null)를 설정합니다.
    StateCache.SetShaderResourceView<ShaderFrequency>(SRV, ResourceIndex);
}