#include "Test.hlsli"

struct WVP
{
    float4x4 WVP;
    float4x4 World;
};
ConstantBuffer<WVP> gWVP : register(b1);

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    output.position = mul(input.position, gWVP.WVP);
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float3x3) gWVP.World));
    output.worldPosition = mul(input.position, gWVP.World).xyz;
    
    return output;
}