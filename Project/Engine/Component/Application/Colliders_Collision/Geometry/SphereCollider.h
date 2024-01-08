#pragma once
#include "Geometry/Common/BaseCollider.h"
#include "CollisionPrimitive.h"
#include "Geometry/Common/BaseShapeCollider.h"

class SphereCollider :public BaseCollider, public SphereCP, public BaseShapeCollider
{
public:
	static SphereCollider* Create(GeometryBase* object, Vector3 offset = {0,0,0}, float radius = 1);

public:
	SphereCollider(Vector3 offset, float radius){
		offset_ = offset;
		radius_ = radius;

		vertNum_ = kSubdivision_*kSubdivision_*4;
		indexNum_ = kSubdivision_*kSubdivision_*6;

		shapeType_ = COLLISIONSHAPE_SPHERE;
	}

	void Update() override;
	void ShapeDraw(Camera* camera) override;

	//Getter/Setter
	void SetRadius(float radius)	{radius_ = radius;}

private:
	void VertexData();
	void IndexData();

private:
	const uint32_t kSubdivision_ = 16;

	Vector3 offset_;
	float radius_;
};

