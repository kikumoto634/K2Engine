#include "Sphere_Collision.h"

void Sphere_Collider::Update()
{
	const Matrix4x4 matWorld = objObject->GetWorldMatrix();

	//球のメンバ変数更新
	Sphere_ColliderStruct::center = Vector3(matWorld.m[3][0] + offset.x, matWorld.m[3][1] + offset.y, matWorld.m[3][2] + offset.z);
	Sphere_ColliderStruct::radius = radius;
}
