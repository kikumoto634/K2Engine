#pragma once
#include "Transform.h"

class GeometryBaseCollider;

class Collider
{
public:
	void ColliderUpdate();
	void ColliderDraw(Vector3 pos, Matrix4x4 viewProjectionMatrix);

public:
	virtual ~Collider() = default;
	virtual void OnCollision(){};

	//Getter/Setter
	virtual Vector3 GetColliderCenterPos()	const = 0;
	float GetRadius()	{return radius;}

	void SetRadius(float r)	{radius = r;}

private:
	GeometryBaseCollider* colliderModel = nullptr;
	float radius = 1.0f;
};

