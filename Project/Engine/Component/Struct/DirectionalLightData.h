#pragma once
#include "Vector4.h"
#include "Vector3.h"

//平行高原
struct DirectionalLight{
	Vector4 color;
	Vector3 direction;
	float intensity;
};