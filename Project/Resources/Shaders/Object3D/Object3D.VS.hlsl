#include "Object3D.hlsli"

VertexShaderOutput main( VertexShaderInput input )
{
    VertexShaderOutput output;
    output.position = mul(input.position, WVP);
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float3x3) World));
    
    //float4 pos = mul(input.position, World);
    //pos = mul(pos, lightVP);
    //pos.xyz = pos.xyz / pos.w;
    //output.positionSM.x = (1.0f + pos.x) / 2.0f;
    //output.positionSM.y = (1.0f - pos.y) / 2.0f;
    //output.positionSM.z = pos.z;
    
	return output;
}