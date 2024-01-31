struct ParticleForGPU
{
    float4x4 WVP;
};
ConstantBuffer<ParticleForGPU> gParticleForGPU : register(b0);

struct VertexShaderOutput
{
    float4 position : SV_POSITION;
};

struct VertexShaderInput
{
    float4 position : POSITION0;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    float4 pos = input.position;
    output.position = mul(pos, gParticleForGPU.WVP);
    return output;
}