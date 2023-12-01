#pragma once
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"

Vector3 Multiplication(const Vector3& v, const Matrix4x4& m);
Vector4 Multiplication(const Vector4& v, const Matrix4x4& m);