#include "Object3D.hlsli"

float4 main(/*VertexShaderInput input*/) : SV_POSITION
{
    //float4 pos = float4(input.position.xyz,1.0f);
    //pos = mul(pos, gTransformationMatrix.World);
    //pos = mul(pos, gDirectionalLight.);
    return (1, 1, 1, 1);
}

//モデルの行列とライトの行列が必要
//二つともGeometryBaseのRootSignatureで設定しているので
//ShadowMap専用のCommonとPipelineを作成するクラスの作成
//そこで必要な情報をシェーダーに送れるようにする
//それと同時にLightクラスにて、Directionalを改造、ライトから見たときの行列の作成
//VPなので、カメラなどでどのように作成されているのか
//座標、回転値を逆数に掛けて、View,
//平行投影でprojection
//両方を掛けてVP、それを代入しよう