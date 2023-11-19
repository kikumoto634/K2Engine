#pragma once
#include "Common/GeometryBase.h"
class Line : public GeometryBase
{
public:
	static Line* Create(Vector3 start = {0,0,0}, Vector3 end = {1,0,0});

public:
	Line(Vector3 start, Vector3 end){
		translate = start;
		target = end;
		rotation = {0,0,0};
		scale = {0,0,0};

		vertNum_ = 2;

		PSPath_ = "Object3D/Object3D.PS.OneColor.hlsl";

		primitiveType_ = PrimitiveType::LINE;
	}

	void Update();

private:
	void LineVertexData();

private:
	Vector3 target = {-1.f,0,0.f};
};