#pragma once
#include "Common/GeometryBaseCollider.h"
#include "Collider.h"
#include "CollisionPrimitive.h"

class SphereCollider : public GeometryBaseCollider,public Collider, public SphereCP
{
public:
	static SphereCollider* Create(Vector3 offset = {0,0,0}, float radius = 1);

public:
	SphereCollider(Vector3 offset, float radius){
		offset_ = offset;
		radius_ = radius;

		vertNum_ = kSubdivision_*kSubdivision_*4;
		indexNum_ = kSubdivision_*kSubdivision_*6;

		shapeType_ = COLLISIONSHAPE_SPHERE;
	}

	void Update() override;

	//Getter/Setter
	void SetRadius(float radius)	{radius_ = radius;}

private:
	void VertexData();
	void IndexData();

private:
	//球体分割数
	const uint32_t kSubdivision_ = 16;

	Vector3 offset_;
	float radius_;
};

