


float4 VS(float3 Position : ATTRIBUTE0) : SV_Position
{
    return float4(Position.xyz, 15.0f);
}