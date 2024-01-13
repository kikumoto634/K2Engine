struct ParticleForGPU
{
    float4x4 WVP;
};
StructuredBuffer<ParticleForGPU> gParticleForGPU : register(t0);

struct VertexShaderInput
{
    float4 position : POSITION0;
};

struct VertexShaderOutput
{
    float4 position : SV_POSITION;
};

VertexShaderOutput main(VertexShaderInput input, uint instanceId : SV_InstanceID)
{
    VertexShaderOutput output;
    output.position = mul(input.position, gParticleForGPU[instanceId].WVP);
    
    return output;
}