#pragma once
#include "Matrix4x4.h"
#include "Vector4.h"

struct ParticleForGPUData{
	Matrix4x4 WVP;		//ワールドビュープロジェクション
	Matrix4x4 World;	//ワールド
	Vector4 color;
};