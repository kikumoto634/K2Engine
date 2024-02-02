#pragma once
#include "Vector4.h"
#include "Vector3.h"
#include "Vector2.h"

//頂点データ
struct VertexData{
	Vector4 position;	//座標
	Vector2 texcoord;	//UV
	Vector3 normal;		//法線
};