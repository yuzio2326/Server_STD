
struct VSInput_PositionUV
{
    float3 Position : ATTRIBUTE0;
    float2 UV : ATTRIBUTE1;
};

struct VSOutput_PositionUV
{
    float4 SVPosition : SV_POSITION;
    float2 UV : UV;
};
