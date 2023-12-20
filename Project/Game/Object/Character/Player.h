#pragma once
#include "../Geometry/ObjModel.h"
#include "Collider/BaseCollider.h"

class Player : public ObjModel ,public BaseCollider
{
public:
	Player(std::string filePath, Transform transform = {{0,0,0}, {0,0,0}, {0.5f,0.5f,0.5f}});

public:
	void Update()override;

	void OnCollision(const CollisionInfo& info) override;
};

