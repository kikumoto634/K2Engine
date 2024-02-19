#include "Test.hlsli"

struct WVP
{
    float4x4 WVP;
    float4x4 World;
};
StructuredBuffer<WVP> gWVP : register(t0);

VertexShaderOutput main(VertexShaderInput input, uint instanceId : SV_InstanceID)
{
    VertexShaderOutput output;
    output.position = mul(input.position, gWVP[instanceId].WVP);
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float3x3) gWVP[instanceId].World));
    output.worldPosition = mul(input.position, gWVP[instanceId].World).xyz;
    output.SV_InstanceID = instanceId;
    
    return output;
}