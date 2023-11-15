#pragma once
#include "Matrix4x4.h"

//行列
struct TransformationMatrix{
	Matrix4x4 WVP;		//ワールドビュープロジェクション
	Matrix4x4 World;	//ワールド
};