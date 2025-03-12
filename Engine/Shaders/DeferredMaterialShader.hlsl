#include "PBRCommon.fxh"

FVSOutput VS(FVSInput Input)
{
    float4 WorldPosition = mul(float4(Input.Position, 1.f), WorldMatrix);
    float4 FinalPosition = mul(WorldPosition, ViewProjectionMatrix);
    
    FVSOutput Output;
    Output.SVPosition = FinalPosition;
    Output.PositionWS = WorldPosition;
    
    Output.NormalWS = normalize(mul(Input.Normal, (float3x3) WorldInverseTransposeMatrix));
    Output.UV = Input.UV;
    
    return Output;
}

// GBuffer
// - SV_Target0: GBufferA: Normal
// - SV_Target1: GBufferB: Albedo(RGB), Specular(A; 우리는 없음)
// - SV_Target2: GBufferC: Metallic(R), Roughness(G), Ambient Occlusion(B)
// - SV_Target3: GBufferD: World Position(RGB)
struct PS_Output
{
    float4 GBufferA : SV_Target0; // Normal
    float4 GBufferB : SV_Target1; // Albedo(RGB), Specular(A; 우리는 없음)
    float4 GBufferC : SV_Target2; // Metallic(R), Roughness(G), Ambient Occlusion(B)
    float4 GBufferD : SV_Target3; // World Position(RGB)
};

PS_Output PS(FVSOutput Input)
{
    PS_Output Output;
    
    // Texture에서 구한 Normal을 -1 ~ 1 범위로 변환한다
    float3 LocalNormal = BiasX2(NormalTexture.Sample(TextureSampler, Input.UV));
    // Texture에서 구한 Normal을 World Space로 변환한다
    float3 NormalWS = PeturbNormal(LocalNormal, Input.PositionWS.xyz, Input.NormalWS, Input.UV);
    
    const float3 BaseColor = BaseColorTexture.Sample(TextureSampler, Input.UV);
    const float Metallic = MetallicTexture.Sample(TextureSampler, Input.UV).r;
    const float Roughness = RoughnessTexture.Sample(TextureSampler, Input.UV).r;
    const float AO = AmbientOcclusionTexture.Sample(TextureSampler, Input.UV).r;
    
    Output.GBufferA = float4(NormalWS, 1.f);
    Output.GBufferB = float4(BaseColor, 1.f);
    Output.GBufferC = float4(Metallic, Roughness, AO, 1.f);
    Output.GBufferD = float4(Input.PositionWS);
    
    return Output;
}