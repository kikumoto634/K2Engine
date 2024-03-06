#pragma once
#include "Base/GeometryBase.h"
class Sphere : public GeometryBase
{
public:
	static Sphere* Create(Transform transform = {{0,0,0}, {0,0,0}, {1,1,1}});

public:
	Sphere(Transform lTransform){
		translate = lTransform.translate;
		rotation = lTransform.rotation;
		scale = lTransform.scale;

		vertNum_ = kSubdivision_*kSubdivision_*4;
		indexNum_ = kSubdivision_*kSubdivision_*6;

		texturePath_ = "monsterBall.png";

		//PSPath_ = "Object3D/Object3D.PS.Texture.hlsl";
	}
	void Update();


private:
	void SphereVertexData();
	void SphereIndexData();

private:
	//球体分割数
	const uint32_t kSubdivision_ = 16;
};

