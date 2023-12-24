#pragma once
#include "../Geometry/ObjModel.h"

class Player : public ObjModel
{
public:
	Player(std::string filePath, Transform transform = {{0,0,0}, {0,0,0}, {0.8f,0.8f,0.8f}});

public:
	void Update()override;
	void OnCollision() override;

	//Getter/Setter
	Vector3 GetColliderCenterPos()	const override{
		return translate;
	};

private:
	float velocity = 0.1f;
};

