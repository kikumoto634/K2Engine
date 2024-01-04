#include "Particle.hlsli"

struct WorldConstantBuffer
{
    float4x4 WVP;
    float4x4 World;
};
StructuredBuffer<WorldConstantBuffer> gWorldConstantBuffer : register(t0);

VertexShaderOutput main(VertexShaderInput input, uint instanceId : SV_InstanceID)
{
    VertexShaderOutput output;
    output.position = mul(input.position, gWorldConstantBuffer[instanceId].WVP);
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float3x3) gWorldConstantBuffer[instanceId].World));
    
    return output;
}