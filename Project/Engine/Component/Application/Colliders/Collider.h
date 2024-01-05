#pragma once
#include "Transform.h"
#include "Camera.h"

class GeometryBaseCollider;

class Collider
{
public:
	void ColliderUpdate();
	void ColliderDraw(Vector3 pos, Camera* camera);

public:
	virtual ~Collider() = default;
	virtual void OnCollision(){};

	//Getter/Setter
	virtual Vector3 GetColliderCenterPos()	const = 0;
	float GetRadius()	{return radius_;}

	void SetRadius(float r)	{radius_ = r;}

private:
	GeometryBaseCollider* colliderModel_ = nullptr;
	float radius_ = 1.0f;
};

