struct ParticleForGPU
{
    float4x4 WVP;
};
StructuredBuffer<ParticleForGPU> gParticleForGPU : register(t0);

struct VertexShaderOutput
{
    float4 position : SV_POSITION;
};

struct VertexShaderInput
{
    float4 position : POSITION0;
};

VertexShaderOutput main(VertexShaderInput input, uint instanceId : SV_InstanceID)
{
    VertexShaderOutput output;
    float4 pos = input.position;
    output.position = mul(pos, gParticleForGPU[instanceId].WVP);
    return output;
}