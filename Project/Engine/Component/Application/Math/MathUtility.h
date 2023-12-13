#pragma once
#include "Vector3.h"
#include "Matrix4x4.h"

//掛け算
Vector3 Multiplication(const Vector3& v, const Matrix4x4& m);

//最短角度補間
float LerpShortAngle(float a, float b, float t);