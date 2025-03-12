#include "RHIUtilites.h"
#include "RHIAccess.h"
#include "RHICommandList.h"

RHI_API EGpuVendorId RHIGetPreferredAdapterVendor()
{
	/*if (FParse::Param(FCommandLine::Get(), TEXT("preferAMD")))
	{
		return EGpuVendorId::Amd;
	}

	if (FParse::Param(FCommandLine::Get(), TEXT("preferIntel")))
	{
		return EGpuVendorId::Intel;
	}

	if (FParse::Param(FCommandLine::Get(), TEXT("preferNvidia")))
	{
		return EGpuVendorId::Nvidia;
	}

	if (FParse::Param(FCommandLine::Get(), TEXT("preferMS")) || FParse::Param(FCommandLine::Get(), TEXT("preferMicrosoft")))
	{
		return EGpuVendorId::Microsoft;
	}*/

	return EGpuVendorId::Unknown;
}


ERHIAccess RHIGetDefaultResourceState(ETextureCreateFlags InUsage, bool bInHasInitialData)
{
	// By default assume it can be bound for reading
	ERHIAccess ResourceState = ERHIAccess::SRVMask;

	if (!bInHasInitialData)
	{
		if (EnumHasAnyFlags(InUsage, TexCreate_RenderTargetable))
		{
			ResourceState = ERHIAccess::RTV;
		}
		else if (EnumHasAnyFlags(InUsage, TexCreate_DepthStencilTargetable))
		{
			ResourceState = ERHIAccess::DSVWrite | ERHIAccess::DSVRead;
		}
		else if (EnumHasAnyFlags(InUsage, TexCreate_UAV))
		{
			ResourceState = ERHIAccess::UAVMask;
		}
		else if (EnumHasAnyFlags(InUsage, TexCreate_Presentable))
		{
			ResourceState = ERHIAccess::Present;
		}
		else if (EnumHasAnyFlags(InUsage, TexCreate_ShaderResource))
		{
			ResourceState = ERHIAccess::SRVMask;
		}
		else if (EnumHasAnyFlags(InUsage, TexCreate_Foveation))
		{
			ResourceState = ERHIAccess::ShadingRateSource;
		}
	}

	return ResourceState;
}

RHI_API ERHIAccess RHIGetDefaultResourceState(EBufferUsageFlags InUsage, bool bInHasInitialData)
{
	// 버퍼 유형마다 기본 읽기 상태가 다릅니다.
	ERHIAccess DefaultReadingState = ERHIAccess::Unknown;
	if (EnumHasAnyFlags(InUsage, BUF_IndexBuffer))
	{
		DefaultReadingState = ERHIAccess::VertexOrIndexBuffer;
	}
	if (EnumHasAnyFlags(InUsage, BUF_VertexBuffer))
	{
		// 버텍스 버퍼 또는 일반 데이터 버퍼일 수 있습니다.
		DefaultReadingState = DefaultReadingState | ERHIAccess::VertexOrIndexBuffer | ERHIAccess::SRVMask;
	}
	if (EnumHasAnyFlags(InUsage, BUF_StructuredBuffer))
	{
		DefaultReadingState = DefaultReadingState | ERHIAccess::SRVMask;
	}

	// 버텍스와 인덱스 버퍼는 BUF_ShaderResource 플래그를 설정하지 않았을 수 있으며 기본적으로 읽을 수 있다고 가정합니다.
	ERHIAccess ResourceState = (!EnumHasAnyFlags(DefaultReadingState, ERHIAccess::VertexOrIndexBuffer)) ? ERHIAccess::Unknown : DefaultReadingState;

	// 초기 데이터가 있는 경우 SRV를 사용합니다. 그러면 버퍼를 샘플링할 수 있습니다.
	if (bInHasInitialData)
	{
		ResourceState = DefaultReadingState;
	}
	else
	{
		if (EnumHasAnyFlags(InUsage, BUF_UnorderedAccess))
		{
			ResourceState = ERHIAccess::UAVMask;
		}
		else if (EnumHasAnyFlags(InUsage, BUF_ShaderResource))
		{
			ResourceState = DefaultReadingState | ERHIAccess::SRVMask;
		}
	}

	_ASSERT(ResourceState != ERHIAccess::Unknown);

	return ResourceState;
}
