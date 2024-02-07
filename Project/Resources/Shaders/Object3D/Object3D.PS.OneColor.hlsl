#include "Object3D.hlsli"

struct Material
{
    float4 color;
    int enableLighting;
    float4x4 uvTransform;
    float shininess;
};
ConstantBuffer<Material> gMaterial : register(b2);

//Camera
struct Camera
{
    float3 worldPosition;
};
ConstantBuffer<Camera> gCamera : register(b3);


struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
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
        
        //Condition Light from Eye
        float RdotE = dot(reflectLight, toEye);
        //ReflectdPower
        float specularPow = pow(saturate(RdotE), gMaterial.shininess);

        
        float3 diffuse =
        gMaterial.color.rgb * col * cos * lightIntensity;
        float3 specular =
        col * lightIntensity * specularPow * float3(1.0f, 1.0f, 1.0f);
        //Com
        output.color.rgb = diffuse + specular;
        //alpha
        output.color.a = gMaterial.color.a;
    }
    else
    {
        output.color = gMaterial.color ;
    }
    
    return output;
}