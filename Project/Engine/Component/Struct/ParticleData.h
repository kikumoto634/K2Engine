#pragma once
#include "Transform.h"

struct ParticleData{
	Transform transform;
	Vector3 velocity;
	Vector4 color;
	float lifeTime;
	float currentTime;
};