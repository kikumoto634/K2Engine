#include "Object3D.hlsli"

struct TransformationMatrix
{
    float4x4 WVP;
    float4x4 World;
};
ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b1);

float4 main(VertexShaderInput input) : SV_POSITION
{
    float4 pos = float4(input.position.xyz,1.0f);
    pos = mul(pos, gTransformationMatrix.World);
    pos = mul(pos, lightVP);
    return pos;
}
