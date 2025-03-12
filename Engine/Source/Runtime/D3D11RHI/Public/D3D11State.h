/*=============================================================================
	D3D11State.h: D3D state definitions.
=============================================================================*/

#pragma once
#include "RHIResources.h"


class FD3D11SamplerState : public FRHISamplerState
{
public:

	TRefCountPtr<ID3D11SamplerState> Resource;
};

class FD3D11RasterizerState : public FRHIRasterizerState
{
public:

	TRefCountPtr<ID3D11RasterizerState> Resource;
};

class FD3D11DepthStencilState : public FRHIDepthStencilState
{
public:

	TRefCountPtr<ID3D11DepthStencilState> Resource;

	/* DSV의 개별 깊이 및 스텐실 구성 요소의 읽기/쓰기 상태를 설명합니다. */
	FExclusiveDepthStencil AccessType;
};

//class FD3D11BlendState : public FRHIBlendState
//{
//public:
//
//	TRefCountPtr<ID3D11BlendState> Resource;
//};
//
