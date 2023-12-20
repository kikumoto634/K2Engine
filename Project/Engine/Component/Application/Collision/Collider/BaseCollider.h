#pragma once
#include "Vector3.h"
#include "CollisionAttribute.h"

class BaseCollider
{
public:
	virtual void OnCollision(){};

	//Getter
	Vector3 GetCenter()	{return center;}
	float GetR()	{return radius;}
	uint32_t GetCollisionAttribute()	{return collisionAttribute_;}
	uint32_t GetCollisionMask()	{return collisionMask_;}

	//Setter
	void SetCenter(Vector3 pos)	{center = pos;}
	void SetR(float r)	{radius = r;}
	void SetCollisionAttribute(uint32_t attribute)	{collisionAttribute_ = attribute;}
	void SetCollisionMask(uint32_t mask)	{collisionMask_ = mask;}

private:
	Vector3 center = {0,0,0};
	float radius = 1.0f;

	//属性(my)
	uint32_t collisionAttribute_ = kDefaultCollisionAttribute;
	//マスク(衝突対象)
	uint32_t collisionMask_ = kDefaultCollisionAttribute;

};

