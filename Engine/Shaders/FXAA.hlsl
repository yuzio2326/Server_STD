// FXAA Vertex Shader
#include "Common.fxh"

VSOutput_PositionUV VS(VSInput_PositionUV Input)
{
    VSOutput_PositionUV Output;

    // 화면 사각형의 위치와 텍스처 좌표를 설정
    Output.SVPosition = float4(Input.Position, 1.0);
    Output.UV = Input.UV;

    return Output;
}

// FXAA Pixel Shader
Texture2D SceneTexture : register(t0);
SamplerState Sampler : register(s0);

cbuffer FFXAAUniformBuffer
{
    float2 ScreenSize;
};

float3 PS(VSOutput_PositionUV Input) : SV_TARGET
{
    // 주어진 텍스처 좌표(UV)에서 텍스처를 샘플링하여 현재 픽셀의 색상 정보를 가져옴
    float3 Color = SceneTexture.Sample(Sampler, Input.UV).rgb;
    
    // 주위 픽셀 샘플링 (오른쪽, 왼쪽, 위쪽, 아래쪽)
    float3 TexelRight = SceneTexture.Sample(Sampler, Input.UV + float2(1.0 / ScreenSize.x, 0)).rgb; // 오른쪽 픽셀
    float3 TexelLeft = SceneTexture.Sample(Sampler, Input.UV - float2(1.0 / ScreenSize.x, 0)).rgb; // 왼쪽 픽셀
    float3 TexelUp = SceneTexture.Sample(Sampler, Input.UV + float2(0, 1.0 / ScreenSize.y)).rgb; // 위쪽 픽셀
    float3 TexelDown = SceneTexture.Sample(Sampler, Input.UV - float2(0, 1.0 / ScreenSize.y)).rgb; // 아래쪽 픽셀
    
    // 주위 픽셀과 현재 픽셀 비교 (밝기 계산)
    // 루마(Luma)란, 이미지나 비디오에서 각 픽셀의 밝기를 나타내는 값이야.
    // RGB 색상 값을 특정 가중치를 사용하여 계산함.
    // 이 가중치는 인간의 눈이 각각의 색상을 얼마나 민감하게 인식하는지를 반영해.
    // 0.299, 0.587, 0.114 값의 의미:
    // - 0.299 (Red): 인간의 눈이 빨간색을 중간 정도로 민감하게 인식함.
    // - 0.587 (Green): 인간의 눈이 초록색을 가장 민감하게 인식함.
    // - 0.114 (Blue): 인간의 눈이 파란색을 가장 적게 민감하게 인식함.
    float Luma = dot(Color, float3(0.299, 0.587, 0.114)); // 현재 픽셀의 밝기
    float LumaRight = dot(TexelRight, float3(0.299, 0.587, 0.114)); // 오른쪽 픽셀의 밝기
    float LumaLeft = dot(TexelLeft, float3(0.299, 0.587, 0.114)); // 왼쪽 픽셀의 밝기
    float LumaUp = dot(TexelUp, float3(0.299, 0.587, 0.114)); // 위쪽 픽셀의 밝기
    float LumaDown = dot(TexelDown, float3(0.299, 0.587, 0.114)); // 아래쪽 픽셀의 밝기
    
    // 루마 값 기반 가중치 계산 (최솟값과 최댓값)
    // 밝기 값 중에서 가장 어두운 값과 가장 밝은 값을 찾음.
    float LumaMin = min(min(LumaLeft, LumaRight), min(LumaUp, LumaDown)); // 주위 픽셀들 중 밝기 최솟값
    float LumaMax = max(max(LumaLeft, LumaRight), max(LumaUp, LumaDown)); // 주위 픽셀들 중 밝기 최댓값
    float EdgeThreshold = LumaMax - LumaMin; // 경계 값 계산 (최댓값 - 최솟값)

    // 루마를 사용하는 이유:
    // 1. 에지 감지(Edge Detection): 루마 값은 이미지의 밝기 변화를 감지하는 데 매우 유용해.
    //    이는 에일리어싱(계단 현상)이 주로 발생하는 에지에서 특히 중요해.
    // 2. 간단한 계산: 루마 값은 RGB 값을 하나의 값으로 변환하는 간단한 계산으로 얻을 수 있어.
    // 3. 인간의 시각에 근접한 처리: 루마 값은 인간의 눈이 밝기를 인식하는 방식과 유사하게 동작해.

    // 경계 값 기반으로 색상 혼합 (경계가 있을 경우 주변 픽셀 색상 혼합)
    // 경계 값이 특정 기준(0.2)보다 크면
    if (EdgeThreshold > 0.1) // 경계 값을 조금 더 높게 설정하여 민감도를 낮춤
    {
        // 주변 픽셀 색상들과 현재 픽셀 색상을 혼합하여 부드럽게 만듦
        // 주변 픽셀 색상과 현재 픽셀 색상의 혼합 비율을 조정함
        Color = (Color * 0.6 + (TexelRight + TexelLeft + TexelUp + TexelDown) * 0.25 * 0.4);
        //Color = float4(1.f, 0.f, 0.f, 1.f);
    }
    
    // 최종 색상 반환
    return Color;
}