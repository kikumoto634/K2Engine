#include "Object3D.hlsli"

struct TransformationMatrix
{
    float4x4 WVP;
    float4x4 World;
};
ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b1);

VertexShaderOutput main( VertexShaderInput input )
{
    VertexShaderOutput output;
    output.position = mul(input.position, gTransformationMatrix.WVP);
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float3x3) gTransformationMatrix.World));
    
    float4 pos = mul(input.position, gTransformationMatrix.World);
    pos = mul(pos, lightVP);
    pos.xyz = pos.xyz / pos.w;
    output.positionSM.x = (1.0f + pos.x) / 2.0f;
    output.positionSM.y = (1.0f - pos.y) / 2.0f;
    output.positionSM.z = pos.z;
    
	return output;
}