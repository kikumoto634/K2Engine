#include "Camera.h"
#include "WindowsApp.h"

#include <imgui.h>

Camera::Camera(Transform transform)
{
	translate = transform.translate;
	rotation = transform.rotation;
}

void Camera::Update()
{
#ifdef _DEBUG
	ImGui::Text("camera");
	ImGui::DragFloat3("pos", &translate.x, 0.01f);
	ImGui::DragFloat3("rot", &rotation.x, 0.01f);
	ImGui::DragFloat("Aspect", &aspect_, 0.01f);
#endif // _DEBUG
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
		MakePerspectiveFovMatrix(
			aspect_,
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
