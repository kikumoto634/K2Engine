#pragma once
#include "Geometry/Common/GeometryBase.h"
#include "Vector3.h"

/// <summary>
/// 衝突情報
/// </summary>
struct CollisionInfo
{
public:
	CollisionInfo(GeometryBase* objObject, GeometryBase* collider, const Vector3& inter){
		this->objObject = objObject;
		this->collider = collider;
		this->inter = inter; 
	}

public:
	//衝突相手のオブジェクト
	GeometryBase* objObject = nullptr;
	//衝突相手のコライダー
	GeometryBase* collider = nullptr;
	//衝突点
	Vector3 inter;
};