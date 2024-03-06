#include "Object3D.hlsli"

struct Material
{
    float4 color;
    int enableLighting;
    float4x4 uvTransform;
    float shininess;
};
ConstantBuffer<Material> gMaterial : register(b2);

//カメラ
struct Camera
{
    float3 worldPosition;
};
ConstantBuffer<Camera> gCamera : register(b3);


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

    if (textureColor.a == 0.0f)
    {
        discard;
    }
    if (output.color.a == 0.0f)
    {
        discard;
    }
    

    if (gMaterial.enableLighting != 0)
    {
        float3 dir = normalize(lightDirection).xyz;
        float3 col = normalize(lightColor).rgb;
        
        //Condition Light
        float NdotL = dot(normalize(input.normal), -dir);
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        
        //EyeVector
        float3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
        //ReflectLight
        float3 reflectLight = reflect(dir, normalize(input.normal));
        
        //EyeVector + direction 
        float3 halfVector = normalize(-dir + toEye);
        //Condition Light from Eye
        float NDotH = dot(normalize(input.normal), halfVector);
        //ReflectdPower
        float specularPow = pow(saturate(NDotH), gMaterial.shininess);

        
        float3 diffuse =
        gMaterial.color.rgb * textureColor.rgb * col * cos * lightIntensity;
        float3 specular =
        col * lightIntensity * specularPow * float3(1.0f, 1.0f, 1.0f);
        //Com
        output.color.rgb = diffuse + specular;
        //Alpha
        output.color.a = gMaterial.color.a * textureColor.a;
    }
    else
    {
        output.color = gMaterial.color * textureColor;
    }
    
    return output;
}