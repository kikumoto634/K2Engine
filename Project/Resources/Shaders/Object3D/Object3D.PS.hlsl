#include "Object3D.hlsli"

struct Material
{
    float4 color;
};

Texture2D<float4> gTexture : register(t0);      //SRVのレジスタ
SamplerState gSampler : register(s0);           //Samplerレジスタ

ConstantBuffer<Material> gMaterial : register(b0);
struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    //サンプラー(画像)を基に、UVに応じたピクセルをTexture(ピクセル)に抽出
    float4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    
    PixelShaderOutput output;
    output.color = gMaterial.color * textureColor;
    return output;
}