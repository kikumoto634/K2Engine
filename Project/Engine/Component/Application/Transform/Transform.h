#pragma once
#include "Matrix4x4.h"

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

class Transform
{
public:
	Vector3 translate;
	Vector3 rotation;
	Vector3 scale;

public:
	Transform();
	Transform(Vector3 trans, Vector3 rot, Vector3 scale);

	//Getter
	Matrix4x4 GetWorldMatrix();
	Matrix4x4 GetScaleMatrix();
	Matrix4x4 GetRotMatrix();
	Matrix4x4 GetTransMatrix();

private:
	Matrix4x4 worldMatrix4x4_;
};

