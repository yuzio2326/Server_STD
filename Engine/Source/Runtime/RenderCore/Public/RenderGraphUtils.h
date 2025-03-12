#pragma once

#include "RHI.h"
#include "RHICommandList.h"
#include "RHIDefinitions.h"

/** MSAA에 사용되는 간단한 RDG 텍스처 쌍. */
struct FRDGTextureMSAA
{
    // 우리는 FRDGTextureRef가 없으므로 FTextureRHIRef로 처리합니다

    FRDGTextureMSAA() = default;

    FRDGTextureMSAA(FTextureRHIRef InTarget, FTextureRHIRef InResolve)
        : Target(InTarget)
        , Resolve(InResolve)
    {
    }

    FRDGTextureMSAA(FTextureRHIRef InTexture)
        : Target(InTexture)
        , Resolve(InTexture)
    {
    }

    bool IsValid() const
    {
        return Target != nullptr && Resolve != nullptr;
    }

    bool IsSeparate() const
    {
        return Target != Resolve;
    }

    bool operator==(FRDGTextureMSAA Other) const
    {
        return Target == Other.Target && Resolve == Other.Resolve;
    }

    bool operator!=(FRDGTextureMSAA Other) const
    {
        return !(*this == Other);
    }

    FTextureRHIRef Target = nullptr;
    FTextureRHIRef Resolve = nullptr;
};
