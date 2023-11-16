#include "Object3D.hlsli"

struct Material
{
    float4 color;
    int enableLighting;
    float4x4 uvTransform;
};
ConstantBuffer<Material> gMaterial : register(b0);


struct DirectionalLight
{
    float4 color;
    float3 direction;
    float intensity;
};
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);


struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    if (gMaterial.enableLighting != 0)
    {
        float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        output.color = (gMaterial.color) * float4(gDirectionalLight.color.xyz * cos * gDirectionalLight.intensity, 1.0f);
    }
    else
    {
        output.color = gMaterial.color ;
    }
    
    return output;
}