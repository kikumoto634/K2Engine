#pragma once
#include "Vector3.h"
#include "Vector4.h"

/// <summary>
/// 球体
/// </summary>
struct SphereCP{
	//中心座標
	Vector3 center = {0,0,0};
	//半径
	float radius = 1.0f;
};

/// <summary>
/// 平面
/// </summary>
struct PlaneCP{
	//法線ベクトル
	Vector3 normal = {0,1,0};
	//原点からの距離
	float distance = 1.0f;
};

/// <summary>
/// 法線付き三角形
/// </summary>
class TriangleCP{
public:
	//頂点座標
	Vector3 p0;
	Vector3 p1;
	Vector3 p2;
	//法線ベクトル
	Vector3 normal;
};