#include "Common.fxh"
#include "PBRCommon.fxh"

VSOutput_PositionUV VS(VSInput_PositionUV Input)
{
    VSOutput_PositionUV Output;

    // 화면 사각형의 위치와 텍스처 좌표를 설정
    Output.SVPosition = float4(Input.Position, 1.0);
    Output.UV = Input.UV;

    return Output;
}

// GBufferA: Normal
// GBufferB: Albedo(RGB), Specular(A; 우리는 없음)
// GBufferC: Metallic(R), Roughness(G), Ambient Occlusion(B)
// GBufferD: World Position(RGB)
Texture2D GBufferA : register(t0);
Texture2D GBufferB : register(t1);
Texture2D GBufferC : register(t2);
Texture2D GBufferD : register(t3);

float3 PS(VSOutput_PositionUV Input) : SV_TARGET
{
    const float3 NormalWS = GBufferA.Sample(TextureSampler, Input.UV);
    const float3 BaseColor = GBufferB.Sample(TextureSampler, Input.UV);
    const float3 MetallicRoughnessAO = GBufferC.Sample(TextureSampler, Input.UV);
    const float Metallic = MetallicRoughnessAO.r;
    const float Roughness = MetallicRoughnessAO.g;
    const float AmbientOcclusion = MetallicRoughnessAO.b;
    const float3 PositionWS = GBufferD.Sample(TextureSampler, Input.UV);
    
    const float3 V = normalize(EyePosition - PositionWS);
    const float3 Color = LightSurface(V, NormalWS, LightColor.xyz, LightDirection, BaseColor, Roughness, Metallic, AmbientOcclusion);
        
    return Color;

}