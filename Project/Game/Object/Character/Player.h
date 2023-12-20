#pragma once
#include "../Geometry/ObjModel.h"

class Player : public ObjModel
{
public:
	Player(std::string filePath, Transform transform = {{0,0,0}, {0,0,0}, {0.5f,0.5f,0.5f}});

public:
	void Update()override;
	void OnCollision()override;
};

