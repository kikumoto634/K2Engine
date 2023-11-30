#include "Object3D.hlsli"

float4 main(VertexShaderInput input) : SV_POSITION
{
    float4 pos = float4(input.position.xyz,1.0f);
    pos = mul(pos, World);
    pos = mul(pos, lightVP);
    return pos;
}
