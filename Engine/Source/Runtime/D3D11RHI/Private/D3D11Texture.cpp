#include "D3D11RHIPrivate.h"


namespace UE::DXGIUtilities
{
    inline DXGI_FORMAT FindSharedResourceFormat(DXGI_FORMAT InFormat, bool bSRGB)
    {
        if (bSRGB)
        {
            switch (InFormat)
            {
            case DXGI_FORMAT_B8G8R8X8_TYPELESS: return DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;
            case DXGI_FORMAT_B8G8R8A8_TYPELESS: return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
            case DXGI_FORMAT_R8G8B8A8_TYPELESS: return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
            case DXGI_FORMAT_BC1_TYPELESS:      return DXGI_FORMAT_BC1_UNORM_SRGB;
            case DXGI_FORMAT_BC2_TYPELESS:      return DXGI_FORMAT_BC2_UNORM_SRGB;
            case DXGI_FORMAT_BC3_TYPELESS:      return DXGI_FORMAT_BC3_UNORM_SRGB;
            case DXGI_FORMAT_BC7_TYPELESS:      return DXGI_FORMAT_BC7_UNORM_SRGB;
            };
        }
        else
        {
            switch (InFormat)
            {
            case DXGI_FORMAT_B8G8R8X8_TYPELESS: return DXGI_FORMAT_B8G8R8X8_UNORM;
            case DXGI_FORMAT_B8G8R8A8_TYPELESS: return DXGI_FORMAT_B8G8R8A8_UNORM;
            case DXGI_FORMAT_R8G8B8A8_TYPELESS: return DXGI_FORMAT_R8G8B8A8_UNORM;
            case DXGI_FORMAT_BC1_TYPELESS:      return DXGI_FORMAT_BC1_UNORM;
            case DXGI_FORMAT_BC2_TYPELESS:      return DXGI_FORMAT_BC2_UNORM;
            case DXGI_FORMAT_BC3_TYPELESS:      return DXGI_FORMAT_BC3_UNORM;
            case DXGI_FORMAT_BC7_TYPELESS:      return DXGI_FORMAT_BC7_UNORM;
            };
        }

        switch (InFormat)
        {
        case DXGI_FORMAT_R32G32B32A32_TYPELESS: return DXGI_FORMAT_R32G32B32A32_UINT;
        case DXGI_FORMAT_R32G32B32_TYPELESS:    return DXGI_FORMAT_R32G32B32_UINT;
        case DXGI_FORMAT_R16G16B16A16_TYPELESS: return DXGI_FORMAT_R16G16B16A16_UNORM;
        case DXGI_FORMAT_R32G32_TYPELESS:       return DXGI_FORMAT_R32G32_UINT;
        case DXGI_FORMAT_R10G10B10A2_TYPELESS:  return DXGI_FORMAT_R10G10B10A2_UNORM;
        case DXGI_FORMAT_R16G16_TYPELESS:       return DXGI_FORMAT_R16G16_UNORM;
        case DXGI_FORMAT_R8G8_TYPELESS:         return DXGI_FORMAT_R8G8_UNORM;
        case DXGI_FORMAT_R8_TYPELESS:           return DXGI_FORMAT_R8_UNORM;

        case DXGI_FORMAT_BC4_TYPELESS:          return DXGI_FORMAT_BC4_UNORM;
        case DXGI_FORMAT_BC5_TYPELESS:          return DXGI_FORMAT_BC5_UNORM;
        case DXGI_FORMAT_R24G8_TYPELESS:        return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
        case DXGI_FORMAT_R32_TYPELESS:          return DXGI_FORMAT_R32_FLOAT;
        case DXGI_FORMAT_R16_TYPELESS:          return DXGI_FORMAT_R16_UNORM;

        case DXGI_FORMAT_R32G8X24_TYPELESS:     return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
        }

        return InFormat;
    }

    inline DXGI_FORMAT FindDepthStencilResourceFormat(DXGI_FORMAT InFormat)
    {
        switch (InFormat)
        {
        case DXGI_FORMAT_R32_FLOAT: return DXGI_FORMAT_R32_TYPELESS;
        case DXGI_FORMAT_R16_FLOAT: return DXGI_FORMAT_R16_TYPELESS;
        }

        return InFormat;
    }

    inline DXGI_FORMAT GetPlatformTextureResourceFormat(DXGI_FORMAT InFormat, ETextureCreateFlags InFlags)
    {
        // 유효한 공유 텍스처 포맷을 찾습니다.
        if (EnumHasAnyFlags(InFlags, ETextureCreateFlags::Shared))
        {
            return FindSharedResourceFormat(InFormat, EnumHasAnyFlags(InFlags, ETextureCreateFlags::SRGB));
        }
        if (EnumHasAnyFlags(InFlags, ETextureCreateFlags::DepthStencilTargetable))
        {
            return FindDepthStencilResourceFormat(InFormat);
        }

        return InFormat;
    }

    /** 입력 포맷과 SRGB 설정에 적합한 DXGI 포맷을 찾습니다. */
    inline DXGI_FORMAT FindShaderResourceFormat(DXGI_FORMAT InFormat, bool bSRGB)
    {
        if (bSRGB)
        {
            switch (InFormat)
            {
            case DXGI_FORMAT_B8G8R8A8_TYPELESS:    return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
            case DXGI_FORMAT_R8G8B8A8_TYPELESS:    return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
            case DXGI_FORMAT_BC1_TYPELESS:         return DXGI_FORMAT_BC1_UNORM_SRGB;
            case DXGI_FORMAT_BC2_TYPELESS:         return DXGI_FORMAT_BC2_UNORM_SRGB;
            case DXGI_FORMAT_BC3_TYPELESS:         return DXGI_FORMAT_BC3_UNORM_SRGB;
            case DXGI_FORMAT_BC7_TYPELESS:         return DXGI_FORMAT_BC7_UNORM_SRGB;
            };
        }
        else
        {
            switch (InFormat)
            {
            case DXGI_FORMAT_B8G8R8A8_TYPELESS: return DXGI_FORMAT_B8G8R8A8_UNORM;
            case DXGI_FORMAT_R8G8B8A8_TYPELESS: return DXGI_FORMAT_R8G8B8A8_UNORM;
            case DXGI_FORMAT_BC1_TYPELESS:      return DXGI_FORMAT_BC1_UNORM;
            case DXGI_FORMAT_BC2_TYPELESS:      return DXGI_FORMAT_BC2_UNORM;
            case DXGI_FORMAT_BC3_TYPELESS:      return DXGI_FORMAT_BC3_UNORM;
            case DXGI_FORMAT_BC7_TYPELESS:      return DXGI_FORMAT_BC7_UNORM;
            };
        }
        switch (InFormat)
        {
        case DXGI_FORMAT_R24G8_TYPELESS:    return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
        case DXGI_FORMAT_R32_TYPELESS:      return DXGI_FORMAT_R32_FLOAT;
        case DXGI_FORMAT_R16_TYPELESS:      return DXGI_FORMAT_R16_UNORM;
        case DXGI_FORMAT_R32G8X24_TYPELESS: return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
        }
        return InFormat;
    }

    /** 주어진 포맷에 대해 적절한 깊이-스텐실 타겟 가능한 DXGI 포맷을 찾습니다. */
    inline DXGI_FORMAT FindDepthStencilFormat(DXGI_FORMAT InFormat)
    {
        switch (InFormat)
        {
        case DXGI_FORMAT_R24G8_TYPELESS:    return DXGI_FORMAT_D24_UNORM_S8_UINT;
        case DXGI_FORMAT_R32G8X24_TYPELESS: return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
        case DXGI_FORMAT_R32_TYPELESS:      return DXGI_FORMAT_D32_FLOAT;
        case DXGI_FORMAT_R16_TYPELESS:      return DXGI_FORMAT_D16_UNORM;
        };
        return InFormat;
    }

    /**
     * 주어진 포맷에 스텐실 정보가 포함되어 있는지 여부를 반환합니다.
     * 반드시 FindDepthStencilFormat에 의해 반환된 포맷을 전달해야 하며, 이는 typeless 버전이 해당 깊이 스텐실 뷰 포맷으로 변환된 것입니다.
     */
    inline bool HasStencilBits(DXGI_FORMAT InFormat)
    {
        switch (InFormat)
        {
        case DXGI_FORMAT_D24_UNORM_S8_UINT:
        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
            return true;
        };
        return false;
    }
}

// WARP 장치와 BC7 포맷의 문제를 해결하기 위한 워크어라운드
// 서로 다른 포맷(DXGI_FORMAT_BC7_UNORM vs DXGI_FORMAT_BC7_UNORM_SRGB)으로 두 개의 뷰를 생성하면
// d3d10warp.dll 내부에서 두 번째 뷰를 생성할 때 충돌이 발생합니다.
void ApplyBC7SoftwareAdapterWorkaround(bool bSoftwareAdapter, D3D11_TEXTURE2D_DESC& Desc)
{
    // 소프트웨어 어댑터가 활성화된 경우 워크어라운드를 적용합니다.
    if (bSoftwareAdapter)
    {
        // 워크어라운드를 적용할 조건을 확인합니다.
        bool bApplyWorkaround = Desc.Format == DXGI_FORMAT_BC7_TYPELESS
            && Desc.Usage == D3D11_USAGE_DEFAULT
            && Desc.MipLevels == 1
            && Desc.ArraySize == 1
            && Desc.CPUAccessFlags == 0;

        // 조건을 만족하면 D3D11_RESOURCE_MISC_SHARED 플래그를 추가하여 워크어라운드를 적용합니다.
        if (bApplyWorkaround)
        {
            Desc.MiscFlags |= D3D11_RESOURCE_MISC_SHARED;
        }
    }
}


/**
 * Creates a 2D texture optionally guarded by a structured exception handler.
 */
static void SafeCreateTexture2D(ID3D11Device* Direct3DDevice, int32 UEFormat, const D3D11_TEXTURE2D_DESC* TextureDesc, const D3D11_SUBRESOURCE_DATA* SubResourceData, ID3D11Texture2D** OutTexture2D, const TCHAR* DebugName)
{
#if GUARDED_TEXTURE_CREATES
    bool bDriverCrash = true;
    __try
    {
#endif // #if GUARDED_TEXTURE_CREATES
        VERIFYD3D11CREATETEXTURERESULT(
            Direct3DDevice->CreateTexture2D(TextureDesc, SubResourceData, OutTexture2D),
            UEFormat,
            TextureDesc->Width,
            TextureDesc->Height,
            TextureDesc->ArraySize,
            TextureDesc->Format,
            TextureDesc->MipLevels,
            TextureDesc->BindFlags,
            TextureDesc->Usage,
            TextureDesc->CPUAccessFlags,
            TextureDesc->MiscFlags,
            TextureDesc->SampleDesc.Count,
            TextureDesc->SampleDesc.Quality,
            SubResourceData ? SubResourceData->pSysMem : nullptr,
            SubResourceData ? SubResourceData->SysMemPitch : 0,
            SubResourceData ? SubResourceData->SysMemSlicePitch : 0,
            Direct3DDevice,
            DebugName
        );
#if GUARDED_TEXTURE_CREATES
        bDriverCrash = false;
    }
    __finally
    {
        if (bDriverCrash)
        {
            UE_LOG(LogD3D11RHI, Error,
                TEXT("Driver crashed while creating texture: %ux%ux%u %s(0x%08x) with %u mips, PF_ %d"),
                TextureDesc->Width,
                TextureDesc->Height,
                TextureDesc->ArraySize,
                UE::DXGIUtilities::GetFormatString(TextureDesc->Format),
                (uint32)TextureDesc->Format,
                TextureDesc->MipLevels,
                UEFormat
            );
        }
    }
#endif // #if GUARDED_TEXTURE_CREATES
}

FD3D11Texture* FD3D11DynamicRHI::CreateD3D11Texture2D(const FRHITextureCreateDesc& CreateDesc, TArray<D3D11_SUBRESOURCE_DATA> InitialData)
{
	// 텍스처가 3D 텍스처가 아닌지 확인
	_ASSERT(!CreateDesc.IsTexture3D());

    const bool bTextureArray = CreateDesc.IsTextureArray();
    const bool bCubeTexture = CreateDesc.IsTextureCube();
    const uint32 SizeX = CreateDesc.Extent.x;
    const uint32 SizeY = CreateDesc.Extent.y;
    const uint32 SizeZ = bCubeTexture ? CreateDesc.ArraySize * 6 : CreateDesc.ArraySize;
    const EPixelFormat Format = CreateDesc.Format;
    const uint32 NumMips = CreateDesc.NumMips;
    const uint32 NumSamples = CreateDesc.NumSamples;
    const ETextureCreateFlags Flags = CreateDesc.Flags;

    // 텍스처 크기 및 Mip Levels이 유효한지 확인
    _ASSERT(SizeX > 0 && SizeY > 0 && NumMips > 0);

    if (bCubeTexture)
    {
        _ASSERT(SizeX <= GetMaxCubeTextureDimension(), TEXT("Requested cube texture size too large: %i, Max: %i, DebugName: '%s'"), SizeX, GetMaxCubeTextureDimension(), CreateDesc.DebugName ? CreateDesc.DebugName : TEXT(""));
        _ASSERT(SizeX == SizeY);
    }
    else
    {
        // 2D 텍스처 크기가 최대 크기 이내인지 확인
        _ASSERT(SizeX <= GetMax2DTextureDimension(), TEXT("Requested texture2d x size too large: %i, Max: %i, DebugName: '%s'"), SizeX, GetMax2DTextureDimension(), CreateDesc.DebugName ? CreateDesc.DebugName : TEXT(""));
        _ASSERT(SizeY <= GetMax2DTextureDimension(), TEXT("Requested texture2d y size too large: %i, Max: %i, DebugName: '%s'"), SizeY, GetMax2DTextureDimension(), CreateDesc.DebugName ? CreateDesc.DebugName : TEXT(""));
    }

    if (bTextureArray)
    {
        // 아직 텍스처 배열은 지원되지 않음
        _ASSERT(false);
    }

    const bool bSRGB = EnumHasAnyFlags(Flags, TexCreate_SRGB);

    // 플랫폼에 맞는 텍스처 포맷 찾기
    const DXGI_FORMAT PlatformResourceFormat = UE::DXGIUtilities::GetPlatformTextureResourceFormat((DXGI_FORMAT)GPixelFormats[Format].PlatformFormat, Flags);
    const DXGI_FORMAT PlatformShaderResourceFormat = UE::DXGIUtilities::FindShaderResourceFormat(PlatformResourceFormat, bSRGB);
    const DXGI_FORMAT PlatformRenderTargetFormat = UE::DXGIUtilities::FindShaderResourceFormat(PlatformResourceFormat, bSRGB);

    uint32 CPUAccessFlags = 0;
    D3D11_USAGE TextureUsage = D3D11_USAGE_DEFAULT;
    bool bCreateShaderResource = true;

    uint32 ActualMSAAQuality = GetMaxMSAAQuality(NumSamples);
    _ASSERT(ActualMSAAQuality != 0xffffffff);
    _ASSERT(NumSamples == 1 || !EnumHasAnyFlags(Flags, TexCreate_Shared));

    const bool bIsMultisampled = NumSamples > 1;

    if (EnumHasAnyFlags(Flags, TexCreate_CPUReadback))
    {
        _ASSERT(!EnumHasAnyFlags(Flags, TexCreate_RenderTargetable | TexCreate_DepthStencilTargetable | TexCreate_ShaderResource));

        CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        TextureUsage = D3D11_USAGE_STAGING;
        bCreateShaderResource = false;
    }

    if (EnumHasAnyFlags(Flags, TexCreate_CPUWritable))
    {
        CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        TextureUsage = D3D11_USAGE_STAGING;
        bCreateShaderResource = false;
    }

    // 텍스처 설명 설정
    D3D11_TEXTURE2D_DESC TextureDesc;
    ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
    TextureDesc.Width = SizeX;
    TextureDesc.Height = SizeY;
    TextureDesc.MipLevels = NumMips;
    TextureDesc.ArraySize = SizeZ;
    TextureDesc.Format = PlatformResourceFormat;
    TextureDesc.SampleDesc.Count = NumSamples;
    TextureDesc.SampleDesc.Quality = ActualMSAAQuality;
    TextureDesc.Usage = TextureUsage;
    TextureDesc.BindFlags = bCreateShaderResource ? D3D11_BIND_SHADER_RESOURCE : 0;
    TextureDesc.CPUAccessFlags = CPUAccessFlags;
    TextureDesc.MiscFlags = bCubeTexture ? D3D11_RESOURCE_MISC_TEXTURECUBE : 0;

    ApplyBC7SoftwareAdapterWorkaround(Adapter.bSoftwareAdapter, TextureDesc);

    // NV12와 P010 포맷에서는 SRV를 생성하지 않음
    if (Format == PF_NV12 || Format == PF_P010)
    {
        bCreateShaderResource = false;
    }

    if (EnumHasAnyFlags(Flags, TexCreate_DisableSRVCreation))
    {
        bCreateShaderResource = false;
    }

    if (EnumHasAnyFlags(Flags, TexCreate_Shared))
    {
        _ASSERT(false); // 미지원
    }

    if (EnumHasAnyFlags(Flags, TexCreate_GenerateMipCapable))
    {
        TextureDesc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
    }

    // 텍스처 바인드 플래그 설정
    bool bCreateRTV = false;
    bool bCreateDSV = false;
    bool bCreatedRTVPerSlice = false;

    if (EnumHasAnyFlags(Flags, TexCreate_RenderTargetable))
    {
        _ASSERT(!EnumHasAnyFlags(Flags, TexCreate_DepthStencilTargetable | TexCreate_ResolveTargetable));
        TextureDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
        bCreateRTV = true;
    }
    else if (EnumHasAnyFlags(Flags, TexCreate_DepthStencilTargetable))
    {
        _ASSERT(!EnumHasAnyFlags(Flags, TexCreate_RenderTargetable | TexCreate_ResolveTargetable));
        TextureDesc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
        bCreateDSV = true;
    }
    else if (EnumHasAnyFlags(Flags, TexCreate_ResolveTargetable))
    {
        _ASSERT(!EnumHasAnyFlags(Flags, TexCreate_RenderTargetable | TexCreate_DepthStencilTargetable));
        if (Format == PF_DepthStencil || Format == PF_ShadowDepth || Format == PF_D24)
        {
            TextureDesc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
            bCreateDSV = true;
        }
        else
        {
            TextureDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
            bCreateRTV = true;
        }
    }

    if (Format == PF_NV12 || Format == PF_P010)
    {
        bCreateRTV = false;
    }

    if (EnumHasAnyFlags(Flags, TexCreate_UAV))
    {
        TextureDesc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
    }

    if (bCreateDSV && !EnumHasAnyFlags(Flags, TexCreate_ShaderResource))
    {
        TextureDesc.BindFlags &= ~D3D11_BIND_SHADER_RESOURCE;
        bCreateShaderResource = false;
    }

    TRefCountPtr<ID3D11Texture2D> TextureResource;
    TRefCountPtr<ID3D11ShaderResourceView> ShaderResourceView;
    TArray<TRefCountPtr<ID3D11RenderTargetView>> RenderTargetViews;
    TArray<TRefCountPtr<ID3D11DepthStencilView>> DepthStencilViews;
    DepthStencilViews.resize(FExclusiveDepthStencil::MaxIndex);

    TArray<D3D11_SUBRESOURCE_DATA> SubResourceData;
    D3D11_SUBRESOURCE_DATA const* pSubresourceData = nullptr;

    if (InitialData.size())
    {
        _ASSERT(InitialData.size() == NumMips * SizeZ);
        pSubresourceData = InitialData.data();
    }
    else if (CreateDesc.BulkData)
    {
        uint8* Data = (uint8*)CreateDesc.BulkData->GetResourceBulkData();
        SubResourceData.resize(NumMips * SizeZ);

        uint32 SliceOffset = 0;
        for (uint32 ArraySliceIndex = 0; ArraySliceIndex < SizeZ; ++ArraySliceIndex)
        {
            uint32 MipOffset = 0;
            for (uint32 MipIndex = 0; MipIndex < NumMips; ++MipIndex)
            {
                uint32 DataOffset = SliceOffset + MipOffset;
                uint32 SubResourceIndex = ArraySliceIndex * NumMips + MipIndex;

                uint32 NumBlocksX = FMath::Max<uint32>(1, ((SizeX >> MipIndex) + GPixelFormats[Format].BlockSizeX - 1) / GPixelFormats[Format].BlockSizeX);
                uint32 NumBlocksY = FMath::Max<uint32>(1, ((SizeY >> MipIndex) + GPixelFormats[Format].BlockSizeY - 1) / GPixelFormats[Format].BlockSizeY);

                SubResourceData[SubResourceIndex].pSysMem = &Data[DataOffset];
                SubResourceData[SubResourceIndex].SysMemPitch = NumBlocksX * GPixelFormats[Format].BlockBytes;
                SubResourceData[SubResourceIndex].SysMemSlicePitch = NumBlocksX * NumBlocksY * SubResourceData[MipIndex].SysMemPitch;

                MipOffset += NumBlocksY * SubResourceData[MipIndex].SysMemPitch;
            }
            SliceOffset += MipOffset;
        }

        pSubresourceData = SubResourceData.data();
    }

#if INTEL_EXTENSIONS
    if (EnumHasAnyFlags(Flags, ETextureCreateFlags::Atomic64Compatible) && IsRHIDeviceIntel() && GRHISupportsAtomicUInt64)
    {
        INTC_D3D11_TEXTURE2D_DESC IntelDesc{};
        IntelDesc.EmulatedTyped64bitAtomics = true;
        IntelDesc.pD3D11Desc = &TextureDesc;

        VERIFYD3D11RESULT(INTC_D3D11_CreateTexture2D(IntelExtensionContext, &IntelDesc, pSubresourceData, TextureResource.GetInitReference()));
    }
    else
#endif
    {
        SafeCreateTexture2D(Direct3DDevice, Format, &TextureDesc, pSubresourceData, TextureResource.GetInitReference(), CreateDesc.DebugName);
    }

    if (bCreateRTV)
    {
        for (uint32 MipIndex = 0; MipIndex < NumMips; MipIndex++)
        {
            if (EnumHasAnyFlags(Flags, TexCreate_TargetArraySlicesIndependently) && (bTextureArray || bCubeTexture))
            {
                bCreatedRTVPerSlice = true;

                for (uint32 SliceIndex = 0; SliceIndex < TextureDesc.ArraySize; SliceIndex++)
                {
                    D3D11_RENDER_TARGET_VIEW_DESC RTVDesc = {};

                    RTVDesc.Format = PlatformRenderTargetFormat;

                    if (bIsMultisampled)
                    {
                        RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
                        RTVDesc.Texture2DMSArray.FirstArraySlice = SliceIndex;
                        RTVDesc.Texture2DMSArray.ArraySize = 1;
                    }
                    else
                    {
                        RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
                        RTVDesc.Texture2DArray.FirstArraySlice = SliceIndex;
                        RTVDesc.Texture2DArray.ArraySize = 1;
                        RTVDesc.Texture2DArray.MipSlice = MipIndex;
                    }

                    TRefCountPtr<ID3D11RenderTargetView> RenderTargetView;
                    VERIFYD3D11RESULT_EX(Direct3DDevice->CreateRenderTargetView(TextureResource, &RTVDesc, RenderTargetView.GetInitReference()), Direct3DDevice);
                    RenderTargetViews.push_back(RenderTargetView);
                }
            }
            else
            {
                D3D11_RENDER_TARGET_VIEW_DESC RTVDesc = {};

                RTVDesc.Format = PlatformRenderTargetFormat;

                if (bTextureArray || bCubeTexture)
                {
                    if (bIsMultisampled)
                    {
                        RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
                        RTVDesc.Texture2DMSArray.FirstArraySlice = 0;
                        RTVDesc.Texture2DMSArray.ArraySize = TextureDesc.ArraySize;
                    }
                    else
                    {
                        RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
                        RTVDesc.Texture2DArray.FirstArraySlice = 0;
                        RTVDesc.Texture2DArray.ArraySize = TextureDesc.ArraySize;
                        RTVDesc.Texture2DArray.MipSlice = MipIndex;
                    }
                }
                else
                {
                    if (bIsMultisampled)
                    {
                        RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
                    }
                    else
                    {
                        RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
                        RTVDesc.Texture2D.MipSlice = MipIndex;
                    }
                }

                TRefCountPtr<ID3D11RenderTargetView> RenderTargetView;
                VERIFYD3D11RESULT_EX(Direct3DDevice->CreateRenderTargetView(TextureResource, &RTVDesc, RenderTargetView.GetInitReference()), Direct3DDevice);
                RenderTargetViews.push_back(RenderTargetView);
            }
        }
    }

    if (bCreateDSV)
    {
        D3D11_DEPTH_STENCIL_VIEW_DESC DSVDesc = {};

        DSVDesc.Format = UE::DXGIUtilities::FindDepthStencilFormat(PlatformResourceFormat);

        if (bTextureArray || bCubeTexture)
        {
            if (bIsMultisampled)
            {
                DSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
                DSVDesc.Texture2DMSArray.FirstArraySlice = 0;
                DSVDesc.Texture2DMSArray.ArraySize = TextureDesc.ArraySize;
            }
            else
            {
                DSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
                DSVDesc.Texture2DArray.FirstArraySlice = 0;
                DSVDesc.Texture2DArray.ArraySize = TextureDesc.ArraySize;
                DSVDesc.Texture2DArray.MipSlice = 0;
            }
        }
        else
        {
            if (bIsMultisampled)
            {
                DSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
            }
            else
            {
                DSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
                DSVDesc.Texture2D.MipSlice = 0;
            }
        }

        for (uint32 AccessType = 0; AccessType < FExclusiveDepthStencil::MaxIndex; ++AccessType)
        {
            DSVDesc.Flags = (AccessType & FExclusiveDepthStencil::DepthRead_StencilWrite) ? D3D11_DSV_READ_ONLY_DEPTH : 0;
            if (UE::DXGIUtilities::HasStencilBits(DSVDesc.Format))
            {
                DSVDesc.Flags |= (AccessType & FExclusiveDepthStencil::DepthWrite_StencilRead) ? D3D11_DSV_READ_ONLY_STENCIL : 0;
            }
            VERIFYD3D11RESULT_EX(Direct3DDevice->CreateDepthStencilView(TextureResource, &DSVDesc, DepthStencilViews[AccessType].GetInitReference()), Direct3DDevice);
        }
    }
    _ASSERT(IsValidRef(TextureResource));

    // 셰이더 리소스 뷰 생성
    if (bCreateShaderResource)
    {
        {
            D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};

            SRVDesc.Format = PlatformShaderResourceFormat;

            if (bCubeTexture && bTextureArray)
            {
                SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
                SRVDesc.TextureCubeArray.MostDetailedMip = 0;
                SRVDesc.TextureCubeArray.MipLevels = NumMips;
                SRVDesc.TextureCubeArray.First2DArrayFace = 0;
                SRVDesc.TextureCubeArray.NumCubes = SizeZ / 6;
            }
            else if (bCubeTexture)
            {
                SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
                SRVDesc.TextureCube.MostDetailedMip = 0;
                SRVDesc.TextureCube.MipLevels = NumMips;
            }
            else if (bTextureArray)
            {
                if (bIsMultisampled)
                {
                    SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
                    SRVDesc.Texture2DMSArray.FirstArraySlice = 0;
                    SRVDesc.Texture2DMSArray.ArraySize = TextureDesc.ArraySize;
                }
                else
                {
                    SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
                    SRVDesc.Texture2DArray.MostDetailedMip = 0;
                    SRVDesc.Texture2DArray.MipLevels = NumMips;
                    SRVDesc.Texture2DArray.FirstArraySlice = 0;
                    SRVDesc.Texture2DArray.ArraySize = TextureDesc.ArraySize;
                }
            }
            else
            {
                if (bIsMultisampled)
                {
                    SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
                }
                else
                {
                    SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
                    SRVDesc.Texture2D.MostDetailedMip = 0;
                    SRVDesc.Texture2D.MipLevels = NumMips;
                }
            }
            VERIFYD3D11RESULT_EX(Direct3DDevice->CreateShaderResourceView(TextureResource, &SRVDesc, ShaderResourceView.GetInitReference()), Direct3DDevice);
        }

        _ASSERT(IsValidRef(ShaderResourceView));
    }

    FD3D11Texture* Texture2D = new FD3D11Texture(
        CreateDesc,
        TextureResource,
        ShaderResourceView,
        TextureDesc.ArraySize,
        bCreatedRTVPerSlice,
        RenderTargetViews,
        DepthStencilViews
    );

    if (CreateDesc.BulkData)
    {
        CreateDesc.BulkData->Discard();
    }

    return Texture2D;
}

/**
 * Aligns a value to the nearest higher multiple of 'Alignment'.
 *
 * @param  Val        The value to align.
 * @param  Alignment  The alignment value, can be any arbitrary value.
 *
 * @return The value aligned up to the specified alignment.
 */
template <typename T>
FORCEINLINE constexpr T AlignArbitrary(T Val, uint64 Alignment)
{
    static_assert(TIsIntegral<T>::Value || TIsPointer<T>::Value, "AlignArbitrary expects an integer or pointer type");

    return (T)((((uint64)Val + Alignment - 1) / Alignment) * Alignment);
}

void FD3D11DynamicRHI::RHICopyTexture(FRHITexture* SourceTextureRHI, FRHITexture* DestTextureRHI, const FRHICopyTextureInfo& CopyInfo)
{
    //FRHICommandList_RecursiveHazardous RHICmdList(this);

    FD3D11Texture* SourceTexture = ResourceCast(SourceTextureRHI);
    FD3D11Texture* DestTexture = ResourceCast(DestTextureRHI);

    _ASSERT(SourceTexture && DestTexture);

    //GPUProfilingData.RegisterGPUWork();

    const FRHITextureDesc& SourceDesc = SourceTextureRHI->GetDesc();
    const FRHITextureDesc& DestDesc = DestTextureRHI->GetDesc();

    const uint16 SourceArraySize = SourceDesc.ArraySize * (SourceDesc.IsTextureCube() ? 6 : 1);
    const uint16 DestArraySize = DestDesc.ArraySize * (DestDesc.IsTextureCube() ? 6 : 1);

    const bool bAllPixels =
        SourceDesc.GetSize() == DestDesc.GetSize() && (CopyInfo.Size == FVector3D::Zero || CopyInfo.Size == SourceDesc.GetSize());

    const bool bAllSubresources =
        SourceDesc.NumMips == DestDesc.NumMips && SourceDesc.NumMips == CopyInfo.NumMips &&
        SourceArraySize == DestArraySize && SourceArraySize == CopyInfo.NumSlices;

    if (!bAllPixels || !bAllSubresources)
    {
        const FPixelFormatInfo& PixelFormatInfo = GPixelFormats[SourceTextureRHI->GetFormat()];

        const FVector3D SourceSize = SourceDesc.GetSize();
        FVector3D CopySize = CopyInfo.Size;
        if (CopyInfo.Size == FVector3D::Zero)
        {
            CopySize.x = (uint32)SourceSize.x >> CopyInfo.SourceMipIndex;
            CopySize.y = (uint32)SourceSize.y >> CopyInfo.SourceMipIndex;
            CopySize.z = (uint32)SourceSize.z >> CopyInfo.SourceMipIndex;
        }

        for (uint32 SliceIndex = 0; SliceIndex < CopyInfo.NumSlices; ++SliceIndex)
        {
            uint32 SourceSliceIndex = CopyInfo.SourceSliceIndex + SliceIndex;
            uint32 DestSliceIndex = CopyInfo.DestSliceIndex + SliceIndex;

            for (uint32 MipIndex = 0; MipIndex < CopyInfo.NumMips; ++MipIndex)
            {
                uint32 SourceMipIndex = CopyInfo.SourceMipIndex + MipIndex;
                uint32 DestMipIndex = CopyInfo.DestMipIndex + MipIndex;

                const uint32 SourceSubresource = D3D11CalcSubresource(SourceMipIndex, SourceSliceIndex, SourceTextureRHI->GetNumMips());
                const uint32 DestSubresource = D3D11CalcSubresource(DestMipIndex, DestSliceIndex, DestTextureRHI->GetNumMips());

                D3D11_BOX SrcBox;
                SrcBox.left = (uint32)CopyInfo.SourcePosition.x >> MipIndex;
                SrcBox.top = (uint32)CopyInfo.SourcePosition.y >> MipIndex;
                SrcBox.front = (uint32)CopyInfo.SourcePosition.z >> MipIndex;
                SrcBox.right = AlignArbitrary<uint32>(FMath::Max<uint32>(((uint32)CopyInfo.SourcePosition.x + (uint32)CopySize.x) >> MipIndex, 1), PixelFormatInfo.BlockSizeX);
                SrcBox.bottom = AlignArbitrary<uint32>(FMath::Max<uint32>(((uint32)CopyInfo.SourcePosition.y + (uint32)CopySize.y) >> MipIndex, 1), PixelFormatInfo.BlockSizeY);
                SrcBox.back = AlignArbitrary<uint32>(FMath::Max<uint32>(((uint32)CopyInfo.SourcePosition.z + (uint32)CopySize.z) >> MipIndex, 1), PixelFormatInfo.BlockSizeZ);

                const uint32 DestX = (uint32)CopyInfo.DestPosition.x >> MipIndex;
                const uint32 DestY = (uint32)CopyInfo.DestPosition.y >> MipIndex;
                const uint32 DestZ = (uint32)CopyInfo.DestPosition.z >> MipIndex;

                Direct3DDeviceIMContext->CopySubresourceRegion(DestTexture->GetResource(), DestSubresource, DestX, DestY, DestZ, SourceTexture->GetResource(), SourceSubresource, &SrcBox);
            }
        }
    }
    else
    {
        // 이 경우 매개변수가 모두 기본값인지 확인합니다.
        _ASSERT(CopyInfo.SourceSliceIndex == 0 && CopyInfo.DestSliceIndex == 0 && CopyInfo.SourcePosition == FVector3D::Zero && CopyInfo.DestPosition == FVector3D::Zero &&
            CopyInfo.SourceMipIndex == 0 && CopyInfo.DestMipIndex == 0);
        Direct3DDeviceIMContext->CopyResource(DestTexture->GetResource(), SourceTexture->GetResource());
    }
}