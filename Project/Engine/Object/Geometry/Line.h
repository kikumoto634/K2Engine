#pragma once
#include "Common/GeometryBase.h"
class Line : public GeometryBase
{
public:
	static Line* Create(Transform transform = {{0,0,0}, {0,0,0}, {1,1,1}});

public:
	Line(Transform transform){
		translate = transform.translate;
		rotation = transform.rotation;
		scale = transform.scale;

		vertNum_ = 2;

		PSPath_ = "Object3D/Object3D.PS.OneColor.hlsl";

		pipelinePrimitiveTopology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		commandPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
	}

	void Update();

private:
	void LineVertexData();
};