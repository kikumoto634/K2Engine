#pragma once
#include "Geometry/Common/GeometryBaseCollider.h"

class SphereCollider : public GeometryBaseCollider
{
public:
	static SphereCollider* Create();

public:
	SphereCollider(){
		vertNum_ = kSubdivision*kSubdivision*4;
		indexNum_ = kSubdivision*kSubdivision*6;
	}

private:
	void SphereVertexData();
	void SphereIndexData();

private:
	//球体分割数
	const uint32_t kSubdivision = 16;
};

