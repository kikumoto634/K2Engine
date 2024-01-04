#include "Particle.hlsli"

struct ParticleForGPU
{
    float4x4 WVP;
    float4x4 World;
    float4 color;
};
StructuredBuffer<ParticleForGPU> gParticleForGPU : register(t0);

VertexShaderOutput main(VertexShaderInput input, uint instanceId : SV_InstanceID)
{
    VertexShaderOutput output;
    output.position = mul(input.position, gParticleForGPU[instanceId].WVP);
    output.texcoord = input.texcoord;
    output.color = gParticleForGPU[instanceId].color;
    
    return output;
}