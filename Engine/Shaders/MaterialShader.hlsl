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

//float4 PS(FVSOutput Input) : SV_Target0
//{
//    float3 N = normalize(Input.NormalWS);
//    float3 L = LightDirection;
    
//    float Diffuse = max(dot(N, L), 0.f);
//    return float4((LightColor * Diffuse).xyz, 1.f);
//}

float4 PS(FVSOutput Input) : SV_Target0
{
    // 물체에서 카메라로 향하는 방향 벡터를 계산한다
    const float3 V = normalize(EyePosition - Input.PositionWS.xyz);
    
    // Texture에서 구한 Normal을 -1 ~ 1 범위로 변환한다
    float3 LocalNormal = BiasX2(NormalTexture.Sample(TextureSampler, Input.UV));
    // Texture에서 구한 Normal을 World Space로 변환한다
    float3 NormalWS = PeturbNormal(LocalNormal, Input.PositionWS.xyz, Input.NormalWS, Input.UV);
    
    const float3 BaseColor = BaseColorTexture.Sample(TextureSampler, Input.UV);
    const float Metallic = MetallicTexture.Sample(TextureSampler, Input.UV).r;
    const float Roughness = RoughnessTexture.Sample(TextureSampler, Input.UV).r;
    const float AO = AmbientOcclusionTexture.Sample(TextureSampler, Input.UV).r;
    
    const float3 Color = LightSurface(V, NormalWS, LightColor.xyz, LightDirection, BaseColor, Roughness, Metallic, AO);

    return float4(Color, 1.f);
}