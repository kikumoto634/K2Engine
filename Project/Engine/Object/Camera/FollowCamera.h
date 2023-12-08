#pragma once
#include "Transform.h"

class FollowCamera : public Transform
{
public:
	static FollowCamera* Create(Transform transform = {{0,0,-5}, {0,0,0}, {1,1,1}});

public:
	FollowCamera() = default;
	FollowCamera(Transform transform);

public:
	void Update(Vector3 target, Vector3 offset);

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
	void Move();
	void Rot();

	void ApplyGlobalVariablesInitialize();
	void ApplyGlobalVariablesUpdate();

private:
	Matrix4x4 viewMatrix;
	Matrix4x4 projectionMatrix;

	Matrix4x4 viewProjectionMatrix;

	float aspect_ = 0.45f;
	float speed_ = 0.05f;

	Vector3 target_{};
	Vector3 offset_{};
};

