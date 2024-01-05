#include "Camera.h"
#include "WindowsApp.h"

#include "GlobalVariables.h"

Camera* Camera::instance_ = nullptr;

Camera* Camera::Create(Transform transform)
{
	Camera* instance = new Camera(transform);
	return instance;
}

Camera *Camera::GetInstance()
{
	return instance_;
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
	Vector3 eye = translate;
	Vector3 lookAt = target_;
	Vector3 up = {0,1,0};

	Vector3 cameraAxisZ = Vector3(lookAt - eye).normalize();
	Vector3 cameraAxisX = up.cross(cameraAxisZ).normalize();
	Vector3 cameraAxisY = cameraAxisZ.cross(cameraAxisX);

	float x = -cameraAxisX.dot(eye);
	float y = -cameraAxisY.dot(eye);
	float z = -cameraAxisZ.dot(eye);

	Matrix4x4 matCameraRot = 
	{
		cameraAxisX.x,cameraAxisY.x,cameraAxisZ.x,0,
		cameraAxisX.y,cameraAxisY.y,cameraAxisZ.y,0,
		cameraAxisX.z,cameraAxisY.z,cameraAxisZ.z,0,
		x		 ,y		   ,z	     ,1
	};
	viewMatrix_ = matCameraRot;

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