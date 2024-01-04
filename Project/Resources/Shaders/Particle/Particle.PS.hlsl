#include "Particle.hlsli"

struct Material
{
    float4 color;
    int enableLighting;
    float4x4 uvTransform;
};
ConstantBuffer<Material> gMaterial : register(b0);


Texture2D<float4> gTexture : register(t0); //SRVのレジスタ
SamplerState gSampler : register(s0); //Samplerレジスタ



struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    float4 transformUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, transformUV.xy); //サンプラー(絵)を基に、UVに応じたピクセルをTexture(ピクセル)に抽出
    
    PixelShaderOutput output;
    output.color = gMaterial.color * textureColor;

    if (output.color.a == 0.0f)
    {
        discard;
    }

    output.color = gMaterial.color * textureColor;
    
    return output;
}