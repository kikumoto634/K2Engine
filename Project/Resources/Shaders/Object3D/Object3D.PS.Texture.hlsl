#include "Object3D.hlsli"

struct Material
{
    float4 color;
    int enableLighting;
    float4x4 uvTransform;
};
ConstantBuffer<Material> gMaterial : register(b1);


Texture2D<float4> gTexture : register(t0); //SRVのレジスタ
SamplerState gSampler : register(s0); //Samplerレジスタ

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    float4 transformUV = mul(float4(input.texcoord,0.0f,1.0f), gMaterial.uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, transformUV.xy); //サンプラー(絵)を基に、UVに応じたピクセルをTexture(ピクセル)に抽出
    
    PixelShaderOutput output;
    if (gMaterial.enableLighting != 0)
    {
        float NdotL = dot(normalize(input.normal), -lightDirection);
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        output.color = (gMaterial.color * textureColor) * float4((lightColor.xyz * cos * lightIntensity),1.0f);
    }
    else
    {
        output.color = gMaterial.color * textureColor;
    }
    
    return output;
}