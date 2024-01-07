#pragma once
#include "Transform.h"
#include "Camera.h"
#include "CollisionConfig.h"

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
	uint32_t GetCollisionAttribute()	{return collisionAttribute_;}
	uint32_t GetCollisionMask()	{return collisionMask_;}

	void SetRadius(float r)	{radius_ = r;}
	void SetCollisionAttribute(uint32_t attribute)	{collisionAttribute_ = attribute;}
	void SetCollisionMask(uint32_t mask)	{collisionMask_ = mask;}

private:
	GeometryBaseCollider* colliderModel_ = nullptr;
	float radius_ = 1.0f;

	//衝突属性(自分)
	uint32_t collisionAttribute_ = kCollisionAttributeDefault;
	//衝突マスク(相手)
	uint32_t collisionMask_ = kCollisionAttributeDefault;
};

