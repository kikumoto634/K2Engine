#pragma once
#include <vector>

#include "Vector4.h"
#include "Matrix4x4.h"

#include "Camera.h"

class FrustumCulling
{
public:
	void Initialize();
	void Update(Camera* camera);

public:
	//面
	std::vector<Vector4> planes;

	//頂点
	std::vector<Vector4> points;
};

