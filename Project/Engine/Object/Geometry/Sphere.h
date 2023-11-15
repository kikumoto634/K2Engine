#pragma once
#include "Common/GeometryBase.h"
class Sphere : public GeometryBase
{
public:
	static Sphere* Create(Transform transform = {{0,0,0}, {0,0,0}, {1,1,1}});

public:
	Sphere(Transform transform){
		translate = transform.translate;
		rotation = transform.rotation;
		scale = transform.scale;
		//頂点数決め
		vertNum_ = kSubdivision*kSubdivision*6;
	}
	void Update();

private:
	void SphereVertexData();

private:
	//球体分割数
	const uint32_t kSubdivision = 16;
};

