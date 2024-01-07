#pragma once
#include "Camera.h"
#include "CollisionConfig.h"
#include "CollisionInfo.h"
#include "CollisionShapeTypa.h"
#include "Geometry/Common/GeometryBase.h"

class GeometryBaseCollider;

class Collider
{
//可視化用
public:
	void ColliderUpdate();
	void ColliderDraw(Vector3 pos, Camera* camera);

public:
	Collider() = default;
	virtual ~Collider() = default;

	virtual void Update() = 0;

	virtual void OnCollision(const CollisionInfo& info){
		object_->OnCollision(info);
	};

	//Getter/Setter
	GeometryBase* GetObjects()			{return object_;}
	uint32_t GetCollisionAttribute()	{return collisionAttribute_;}
	uint32_t GetCollisionMask()			{return collisionMask_;}
	CollisionShapeType GetShapeType()	{return shapeType_;}

	void SetObjects(GeometryBase* obj)				{object_ = obj;}
	void SetCollisionAttribute(uint32_t attribute)	{collisionAttribute_ = attribute;}
	void SetCollisionMask(uint32_t mask)			{collisionMask_ = mask;}
	void SetShapeType(CollisionShapeType type)		{shapeType_ = type;}

protected:
	//可視化用
	GeometryBaseCollider* colliderModel_ = nullptr;
	
	//オブジェクト
	GeometryBase* object_ = nullptr;

	//形状タイプ
	CollisionShapeType shapeType_ = SHAPE_UNKNOWN;

	//衝突属性(自分)
	uint32_t collisionAttribute_ = kCollisionAttributeDefault;
	//衝突マスク(相手)
	uint32_t collisionMask_ = kCollisionAttributeDefault;
};

