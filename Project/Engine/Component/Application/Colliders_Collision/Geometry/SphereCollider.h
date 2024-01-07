#pragma once
#include "Common/GeometryBaseCollider.h"

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
	void VertexData();
	void IndexData();

private:
	//球体分割数
	const uint32_t kSubdivision_ = 16;
};

