#pragma once
#include "Vector4.h"
#include "Matrix4x4.h"

//マテリアルデータ
struct GeometryMaterial{
	Vector4 color;			//色
	int enableLighting;		//ライト処理可否
	float padding[3];		//パディング
	Matrix4x4 uvTransform;	//UV行列
	float shininess;		//輝度
};

struct TextureMaterial{
	Vector4 color;			//色
	Matrix4x4 uvTransform;	//UV行列
};