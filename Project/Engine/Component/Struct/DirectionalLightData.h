#pragma once
#include "Vector4.h"
#include "Vector3.h"
#include "Matrix4x4.h"

//平行光源
struct DirectionalLightData{
	Vector4 color;
	Matrix4x4 VP;
	Vector3 direction;
	float intensity;
};