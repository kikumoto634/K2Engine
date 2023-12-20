#pragma once
#include "Vector3.h"

class GeometryBase;
class BaseCollider;

/// <summary>
/// 衝突情報
/// </summary>
struct CollisionInfo
{
public:
	CollisionInfo(GeometryBase* objObject, BaseCollider* collider, const Vector3& inter){
		this->objObject = objObject;
		this->collider = collider;
		this->inter = inter; 
	}

public:
	//衝突相手のオブジェクト
	GeometryBase* objObject = nullptr;
	//衝突相手のコライダー
	BaseCollider* collider = nullptr;
	//衝突点
	Vector3 inter;
};