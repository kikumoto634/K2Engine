#include "Camera.h"
#include "WindowsApp.h"

Camera::Camera(Transform transform)
{
	translate = transform.translate;
	rotation = transform.rotation;
	scale = transform.scale;
}

Camera* Camera::Create(Transform transform)
{
	Camera* instance = new Camera(transform);
	return instance;
}

Matrix4x4 Camera::GetViewMatrix()
{
	viewMatrix = viewMatrix.Inverse(GetWorldMatrix());
	return viewMatrix;
}

Matrix4x4 Camera::GetProjectionMatrix()
{
	projectionMatrix = 
		projectionMatrix.MakePerspectiveFovMatrix(
			0.45f,
			(float)WindowsApp::kWindowWidth_/(float)WindowsApp::kWindowHeight_,
			0.1f,
			100.f
		);
	return projectionMatrix;
}

Matrix4x4 Camera::GetViewProjectionMatrix()
{
	viewProjectionMatrix = GetViewMatrix()*GetProjectionMatrix();
	return viewProjectionMatrix;
}
