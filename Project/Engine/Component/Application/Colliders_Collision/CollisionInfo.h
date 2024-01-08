#pragma once
#include "Vector3.h"

class GeometryBase;
class BaseCollider;

struct CollisionInfo{
public:
	CollisionInfo(GeometryBase* object, BaseCollider* collider, const Vector3& inter){
		object_ = object;
		collider_ = collider;
		inter_ = inter;
	}
public:
	//衝突対象のオブジェクト
	GeometryBase* object_ = nullptr;
	//衝突対象のコライダー
	BaseCollider* collider_ = nullptr;
	//衝突点
	Vector3 inter_;
};