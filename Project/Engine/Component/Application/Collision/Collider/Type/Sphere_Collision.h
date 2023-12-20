#pragma once
#include "Vector3.h"

//構造体
struct Sphere_ColliderStruct
{
	//中心座標
	Vector3 center = {0,0,0};
	//半径
	float radius = 1.0f;
};


#include "../BaseCollider.h"

//クラス
class Sphere_Collider : public BaseCollider, public Sphere_ColliderStruct
{
public:
	Sphere_Collider(Vector3 offset, float radius):
		offset(offset),
		radius(radius)
	{
		shapeType = COLLISIONSHAPE_SPHERE;
	}

	void Update() override;

private:
	Vector3 offset;
	float radius;
};