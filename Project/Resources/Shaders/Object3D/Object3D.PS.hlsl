#include "Object3D.hlsli"

struct Material
{
    float4 color;
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
    float4 textureColor = gTexture.Sample(gSampler, input.texcoord); //サンプラー(絵)を基に、UVに応じたピクセルをTexture(ピクセル)に抽出
    
    PixelShaderOutput output;
    output.color = gMaterial.color * textureColor;
    return output;
}