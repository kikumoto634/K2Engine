#include "Camera.h"
#include "WindowsApp.h"

#include "GlobalVariables.h"

Camera* Camera::Create(Transform transform)
{
	Camera* instance = new Camera(transform);
	return instance;
}

Camera::Camera(Transform transform)
{
	translate = transform.translate;
	rotation = transform.rotation;

	ApplyGlobalVariablesInitialize();
}

void Camera::Update()
{
	ApplyGlobalVariablesUpdate();
}

Matrix4x4 Camera::GetViewMatrix()
{
	viewMatrix_ = viewMatrix_.Inverse(GetWorldMatrix());
	return viewMatrix_;
}

Matrix4x4 Camera::GetProjectionMatrix()
{
	projectionMatrix_ = 
		MakePerspectiveFovMatrix(
			aspect_,
			(float)WindowsApp::kWindowWidth_/(float)WindowsApp::kWindowHeight_,
			0.1f,
			1000.f
		);
	return projectionMatrix_;
}

Matrix4x4 Camera::GetViewProjectionMatrix()
{
	viewProjectionMatrix_ = GetViewMatrix()*GetProjectionMatrix();
	return viewProjectionMatrix_;
}

void Camera::ApplyGlobalVariablesInitialize()
{
#ifdef _DEBUG
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* name = "Camera";
	//グループを追加
	GlobalVariables::GetInstance()->CreateGroup(name);
	globalVariables->AddItem(name, "0.translate", translate);
	globalVariables->AddItem(name, "1.rotate", rotation);
	globalVariables->AddItem(name, "2.scale", scale);
	globalVariables->AddItem(name, "3.aspect", aspect_);
#endif // _DEBUG
}

void Camera::ApplyGlobalVariablesUpdate()
{
#ifdef _DEBUG
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* name = "Camera";
	translate = globalVariables->GetVector3Value(name, "0.translate");
	rotation = globalVariables->GetVector3Value(name, "1.rotate");
	scale = globalVariables->GetVector3Value(name, "2.scale");
	aspect_ = globalVariables->GetFloatValue(name, "3.aspect");
#endif // _DEBUG
}