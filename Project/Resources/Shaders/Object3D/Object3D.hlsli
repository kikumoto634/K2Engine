
cbuffer LightConstantBuffer : register(b0)
{
    float4 lightColor;
    float3 lightDirection;
    float lightIntensity;
}

cbuffer WorldConstantBuffer : register(b1)
{
    float4x4 WVP;
    float4x4 World;
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
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 worldPosition : POSITION0;
};