
cbuffer ConstantBuffer : register(b0)
{
    float4 lightColor;
    float4x4 lightVP;
    float3 lightDirection;
    float lightIntensity;
}


struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float4 positionSM : POSITION_SM;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};