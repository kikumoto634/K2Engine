#pragma once
#include "Geometry/Common/GeometryBaseCollider.h"

class SphereCollider : public GeometryBaseCollider
{
public:
	static SphereCollider* Create();

public:
	SphereCollider(){
		vertNum_ = kSubdivision_*kSubdivision_*4;
		indexNum_ = kSubdivision_*kSubdivision_*6;
	}

private:
	void SphereVertexData();
	void SphereIndexData();

private:
	//球体分割数
	const uint32_t kSubdivision_ = 16;
};

