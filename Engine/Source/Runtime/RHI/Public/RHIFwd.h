#pragma once

#include "Templates/RefCounting.h"

struct FSamplerStateInitializerRHI;
struct FRasterizerStateInitializerRHI;
struct FDepthStencilStateInitializerRHI;

class FRHISamplerState;
class FRHIRasterizerState;
class FRHIDepthStencilState;

class FRHITexture;
class FRHIPixelShader;
class FRHIVertexShader;
class FRHIUniformBuffer;
class FRHIViewport;
class FRHIVertexDeclaration;
class FRHIBoundShaderState;
class FRHIBuffer;

using FSamplerStateRHIRef = TRefCountPtr<FRHISamplerState>;
using FRasterizerStateRHIRef = TRefCountPtr<FRHIRasterizerState>;
using FDepthStencilStateRHIRef = TRefCountPtr<FRHIDepthStencilState>;

using FTextureRHIRef = TRefCountPtr<FRHITexture>;
using FPixelShaderRHIRef = TRefCountPtr<FRHIPixelShader>;
using FVertexShaderRHIRef = TRefCountPtr<FRHIVertexShader>;
using FUniformBufferRHIRef = TRefCountPtr<FRHIUniformBuffer>;
using FViewportRHIRef = TRefCountPtr<FRHIViewport>;
using FVertexDeclarationRHIRef = TRefCountPtr<FRHIVertexDeclaration>;
using FBoundShaderStateRHIRef = TRefCountPtr<FRHIBoundShaderState>;
using FBufferRHIRef = TRefCountPtr<FRHIBuffer>;

struct FVertexElement;
typedef TArray<FVertexElement> FVertexDeclarationElementList;

struct FConstantBufferInfo
{
	FString Name;
	uint8 RegisterIndex = -1;
	uint32 Size = -1;
};
