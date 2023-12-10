#include "Object3D.hlsli"

VertexShaderOutput main( VertexShaderInput input )
{
    VertexShaderOutput output;
    output.position = mul(input.position, WVP);
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float3x3) World));
    
	return output;
}