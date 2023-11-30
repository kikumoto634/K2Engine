#include "Object3D.hlsli"

struct Material
{
    float4 color;
    int enableLighting;
    float4x4 uvTransform;
};
ConstantBuffer<Material> gMaterial : register(b2);


struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    if (gMaterial.enableLighting != 0)
    {
        float NdotL = dot(normalize(input.normal), -lightDirection);
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        output.color = (gMaterial.color) * float4(lightColor.xyz * cos * lightIntensity, 1.0f);
    }
    else
    {
        output.color = gMaterial.color ;
    }
    
    return output;
}