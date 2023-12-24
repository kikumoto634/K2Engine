#pragma once
#include "Transform.h"

class FollowCamera : public Transform
{
public:
	static FollowCamera* Create(Transform transform = {{0,0,-5}, {0,0,0}, {1,1,1}});
	static FollowCamera* GetInstance();

public:
	FollowCamera() = default;
	FollowCamera(Transform transform);

public:
	void Update(Vector3 target);

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
	void Rot();
	void Move();

	void ApplyGlobalVariablesInitialize();
	void ApplyGlobalVariablesUpdate();

private:
	static FollowCamera* instance_;

private:
	Matrix4x4 viewMatrix;
	Matrix4x4 projectionMatrix;

	Matrix4x4 viewProjectionMatrix;

	float aspect_ = 0.45f;
	float speed_ = 0.01f;
	Vector2 RotMinMax = {-45.f,45.f};

	Vector3 target_{};

public:
	Vector3 offset_ = {0,0,-10};
};

