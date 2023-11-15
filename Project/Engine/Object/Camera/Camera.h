#pragma once
#include "Transform.h"

class Camera : public Transform
{
public:
	static Camera* Create(Transform transform = {{0,0,-5}, {0,0,0}, {1,1,1}});

public:
	Camera() = default;
	Camera(Transform transform);

	void Update();

	/// <summary>
	/// カメラ行列
	/// </summary>
	Matrix4x4 GetViewMatrix();
	
	/// <summary>
	/// 射影変換行列
	/// </summary>
	Matrix4x4 GetProjectionMatrix();

	/// <summary>
	/// ビュープロジェクション行列
	/// </summary>
	Matrix4x4 GetViewProjectionMatrix();

private:
	Matrix4x4 viewMatrix;
	Matrix4x4 projectionMatrix;

	Matrix4x4 viewProjectionMatrix;

	float aspect_ = 0.45f;
};

