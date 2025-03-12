#include "D3D11RHIPrivate.h"

/**
 * 캐시에서 버텍스 선언을 조회하는 데 사용되는 키.
 */
struct FD3D11VertexDeclarationKey
{
    /** 선언 내 버텍스 요소들. */
    FD3D11VertexElements VertexElements;
    /** 버텍스 요소들의 해시 값. */
    uint32 Hash;

    uint16 StreamStrides[MaxVertexElementCount] = {};

    /** 초기화 생성자. */
    explicit FD3D11VertexDeclarationKey(const FVertexDeclarationElementList& InElements)
    {
        uint16 UsedStreamsMask = 0;

        for (int32 ElementIndex = 0; ElementIndex < InElements.size(); ElementIndex++)
        {
            const FVertexElement& Element = InElements[ElementIndex];
            D3D11_INPUT_ELEMENT_DESC D3DElement = { 0 };
            D3DElement.InputSlot = Element.StreamIndex;
            D3DElement.AlignedByteOffset = Element.Offset;
            switch (Element.Type)
            {
            case VET_Float1:        D3DElement.Format = DXGI_FORMAT_R32_FLOAT; break;
            case VET_Float2:        D3DElement.Format = DXGI_FORMAT_R32G32_FLOAT; break;
            case VET_Float3:        D3DElement.Format = DXGI_FORMAT_R32G32B32_FLOAT; break;
            case VET_Float4:        D3DElement.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; break;
            case VET_PackedNormal:  D3DElement.Format = DXGI_FORMAT_R8G8B8A8_SNORM; break; //TODO: uint32 doesn't work because D3D11 squishes it to 0 in the IA-VS conversion
            case VET_UByte4:        D3DElement.Format = DXGI_FORMAT_R8G8B8A8_UINT; break; //TODO: SINT, blendindices
            case VET_UByte4N:       D3DElement.Format = DXGI_FORMAT_R8G8B8A8_UNORM; break;
            case VET_Color:         D3DElement.Format = DXGI_FORMAT_B8G8R8A8_UNORM; break;
            case VET_Short2:        D3DElement.Format = DXGI_FORMAT_R16G16_SINT; break;
            case VET_Short4:        D3DElement.Format = DXGI_FORMAT_R16G16B16A16_SINT; break;
            case VET_Short2N:       D3DElement.Format = DXGI_FORMAT_R16G16_SNORM; break;
            case VET_Half2:         D3DElement.Format = DXGI_FORMAT_R16G16_FLOAT; break;
            case VET_Half4:         D3DElement.Format = DXGI_FORMAT_R16G16B16A16_FLOAT; break;
            case VET_Short4N:       D3DElement.Format = DXGI_FORMAT_R16G16B16A16_SNORM; break;
            case VET_UShort2:       D3DElement.Format = DXGI_FORMAT_R16G16_UINT; break;
            case VET_UShort4:       D3DElement.Format = DXGI_FORMAT_R16G16B16A16_UINT; break;
            case VET_UShort2N:      D3DElement.Format = DXGI_FORMAT_R16G16_UNORM; break;
            case VET_UShort4N:      D3DElement.Format = DXGI_FORMAT_R16G16B16A16_UNORM; break;
            case VET_URGB10A2N:     D3DElement.Format = DXGI_FORMAT_R10G10B10A2_UNORM; break;
            case VET_UInt:          D3DElement.Format = DXGI_FORMAT_R32_UINT; break;
            default: E_LOG(Fatal, TEXT("Unknown RHI vertex element type {}"), (uint8)InElements[ElementIndex].Type);
            };
            D3DElement.SemanticName = "ATTRIBUTE";
            D3DElement.SemanticIndex = Element.AttributeIndex;
            //D3DElement.InputSlotClass = Element.bUseInstanceIndex ? D3D11_INPUT_PER_INSTANCE_DATA : D3D11_INPUT_PER_VERTEX_DATA;

            // 스트림에서 읽기 위해 사용되는 인스턴스 인덱스에 적용할 분배기입니다.
            D3DElement.InstanceDataStepRate = 0;// Element.bUseInstanceIndex ? 1 : 0;

            if ((UsedStreamsMask & 1 << Element.StreamIndex) != 0)
            {
                _ASSERT(StreamStrides[Element.StreamIndex] == Element.Stride);
            }
            else
            {
                UsedStreamsMask = UsedStreamsMask | (1 << Element.StreamIndex);
                StreamStrides[Element.StreamIndex] = Element.Stride;
            }

            VertexElements.push_back(D3DElement);
        }

        // 스트림별로 정렬한 후 오프셋으로 정렬.
        struct FCompareDesc
        {
            FORCEINLINE bool operator()(const D3D11_INPUT_ELEMENT_DESC& A, const D3D11_INPUT_ELEMENT_DESC& B) const
            {
                return ((int32)A.AlignedByteOffset + A.InputSlot * std::numeric_limits<uint16_t>::max()) < ((int32)B.AlignedByteOffset + B.InputSlot * std::numeric_limits<uint16_t>::max());
            }
        };
        std::sort(VertexElements.begin(), VertexElements.end(), FCompareDesc());

        // 한 번 해시 계산.
        Hash = MakeHash();
    }

private:
    uint32 MakeHash() const
    {
        uint32 hash = 0;
        // Hash vertex elements
        for (const auto& element : VertexElements)
        {
            hash ^= std::hash<uint32>()(element.AlignedByteOffset) ^
                std::hash<uint32>()(element.InputSlot) ^
                std::hash<uint32>()(element.SemanticIndex);
        }
        // Hash stream strides
        for (int i = 0; i < MaxVertexElementCount; ++i)
        {
            hash ^= std::hash<uint16_t>()(StreamStrides[i]);
        }
        return hash;
    }

public:
    bool operator==(const FD3D11VertexDeclarationKey& Other) const 
    {
        return Hash == Other.Hash;
    }
};

namespace std {
    template<> struct hash<FD3D11VertexDeclarationKey> {
        std::size_t operator()(const FD3D11VertexDeclarationKey& s) const {
            return s.Hash;
        }
    };
}

/** 버텍스 선언의 글로벌 캐시. */
unordered_map<FD3D11VertexDeclarationKey, FVertexDeclarationRHIRef> GVertexDeclarationCache;


FVertexDeclarationRHIRef FD3D11DynamicRHI::RHICreateVertexDeclaration(const FVertexDeclarationElementList& Elements)
{
    // 요소들로부터 키를 구성합니다.
    FD3D11VertexDeclarationKey Key(Elements);

    FVertexDeclarationRHIRef VertexDeclarationRefPtr = nullptr;
    auto It = GVertexDeclarationCache.find(Key);
    if (It == GVertexDeclarationCache.end())
    {
        // Create and add to the cache if it doesn't exist.
        auto Result = GVertexDeclarationCache.emplace(Key, new FD3D11VertexDeclaration(Key.VertexElements, Key.StreamStrides));

        VertexDeclarationRefPtr = Result.first->second;
        VertexDeclarationRefPtr->Hash = Key.Hash;
    }
    else
    {
        VertexDeclarationRefPtr = It->second;
    }

	return VertexDeclarationRefPtr;
}