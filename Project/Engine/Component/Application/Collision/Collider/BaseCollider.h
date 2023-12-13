#pragma once
#include "Type/CollidersShapeType.h"
#include "CollisionInfo.h"

class BaseCollider
{
public:
	void OnCollision(const CollisionInfo& info){
		
	}

	//Getter
	float GetRadius()	{return radius;}
	CollidersShapeType GetShapeType()	{return shapeType;}


	//Setter
	void SetRadius(float r)	{radius = r;}

private:
	float radius = 1.0f;
	//形状タイプ
	CollidersShapeType shapeType = SHAPE_UNKNOWN;
	//属性
	unsigned short attribute = 0b1111111111111111;
};

