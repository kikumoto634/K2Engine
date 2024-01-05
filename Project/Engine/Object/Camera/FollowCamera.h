#pragma once
#include "Camera.h"

class FollowCamera : public Camera
{
public:
	static FollowCamera* Create(Transform transform = {{0,0,-5}, {0,0,0}, {1,1,1}});
	static FollowCamera* GetInstance();

public:
	FollowCamera() = default;
	FollowCamera(Transform transform);

public:
	void Update(Vector3 target);

private:
	void Rot();
	void Move();

	void ApplyGlobalVariablesInitialize() override;
	void ApplyGlobalVariablesUpdate() override;

private:
	static FollowCamera* instance_;

private:
	float speed_ = 0.01f;
	Vector2 RotMinMax_ = {-45.f,45.f};

public:
	Vector3 offset_ = {0,0,-10};
};

