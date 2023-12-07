#pragma once
#include "Common/GeometryBase.h"
class Sphere : public GeometryBase
{
public:
	static Sphere* Create(Transform transform = {{0,0,0}, {0,0,0}, {1,1,1}});

public:
	Sphere(Transform lTransform){
		transform = lTransform;

		vertNum_ = kSubdivision*kSubdivision*4;
		indexNum_ = kSubdivision*kSubdivision*6;

		texturePath_ = "monsterBall.png";

		PSPath_ = "Object3D/Object3D.PS.Texture.hlsl";
	}
	void LocalInitialize();
	void Update();

private:
	void SphereVertexData();
	void SphereIndexData();

private:
	//球体分割数
	const uint32_t kSubdivision = 16;
};

