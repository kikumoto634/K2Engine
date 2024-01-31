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

float GetRandomNumber(float2 texCoord, int Seed)
{
    return frac(sin(dot(texCoord.xy, float2(12.9898, 78.233)) + Seed) * 43758.5453);
}

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    float value = GetRandomNumber(input.position.xy, 10);
    
    float4 pos = input.position;
    pos.x += value * 10;
    pos.y += value * 10;
    //pos.z += value * 100;
    //pos.w += value * 100;
    output.position = mul(pos, gParticleForGPU[].WVP);
    return output;
}