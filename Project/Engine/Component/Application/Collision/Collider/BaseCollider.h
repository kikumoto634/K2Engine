#pragma once
#include "Type/CollidersShapeType.h"
#include "Geometry/Common/GeometryBase.h"
#include "CollisionInfo.h"

class BaseCollider
{
public:
	BaseCollider() = default;
	virtual ~BaseCollider() = default;

public:
	virtual void Update() = 0;

	void OnCollision(const CollisionInfo& info){
		objObject->OnCollision(info);
	}

	//Getter
	GeometryBase* GetObjObject()	{return objObject;}
	CollidersShapeType GetShapeType()	{return shapeType;}

	//Setter
	void SetObjObject(GeometryBase* obj)	{objObject = obj;}

protected:
	GeometryBase* objObject = nullptr;
	//形状タイプ
	CollidersShapeType shapeType = SHAPE_UNKNOWN;
	//属性
	unsigned short attribute = 0b1111111111111111;
};

