#include "RHIResources.h"

FRHIResource::FRHIResource(ERHIResourceType InResourceType)
	: ResourceType(InResourceType)
{
}

FRHIResource::~FRHIResource()
{
}

RHI_API void FRHIResource::Destroy() const
{
	delete this;
}

FRHITexture::FRHITexture(const FRHITextureCreateDesc& InDesc)
	: FRHIViewableResource(RRT_Texture, InDesc.InitialState)
	, TextureDesc(InDesc)
{
	SetName(InDesc.DebugName);
}

RHI_API void FRHITexture::SetName(const FName& InName)
{
	Name = InName;
}

FRHIShader::~FRHIShader()
{
}
