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

		vertNum_ = kSubdivision*kSubdivision*4;
		indexNum_ = kSubdivision*kSubdivision*6;

		PSPath_ = "Object3D/Object3D.PS.OneColor.hlsl";
	}
	void Update();

private:
	void SphereVertexData();
	void SphereIndexData();

private:
	//球体分割数
	const uint32_t kSubdivision = 16;
};
