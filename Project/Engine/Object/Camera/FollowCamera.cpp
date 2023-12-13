#include "FollowCamera.h"
#include "Input.h"
#include "MathUtility.h"
#include "WindowsApp.h"

#include "GlobalVariables.h"
#include <imgui.h>

FollowCamera* FollowCamera::instance_ = nullptr;

FollowCamera *FollowCamera::Create(Transform transform)
{
	if(!instance_){
		instance_ = new FollowCamera(transform);
	}
	return instance_;
}

FollowCamera *FollowCamera::GetInstance()
{
	return instance_;
}

FollowCamera::FollowCamera(Transform transform)
{
	translate = transform.translate;
	rotation = transform.rotation;

	ApplyGlobalVariablesInitialize();
}

void FollowCamera::Update(Vector3 target)
{
	target_ = target;

	Rot();
	Move();

	ApplyGlobalVariablesUpdate();
}

Matrix4x4 FollowCamera::GetViewMatrix()
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
	viewMatrix = matCameraRot;

	return viewMatrix;
}

Matrix4x4 FollowCamera::GetProjectionMatrix()
{
	projectionMatrix = 
		MakePerspectiveFovMatrix(
			aspect_,
			(float)WindowsApp::kWindowWidth_/WindowsApp::kWindowHeight_,
			0.1f,
			1000.f
		);
	return projectionMatrix;
}

Matrix4x4 FollowCamera::GetViewProjectionMatrix()
{
	viewProjectionMatrix = GetViewMatrix()*GetProjectionMatrix();
	return viewProjectionMatrix;
}

void FollowCamera::Rot()
{
	if(!Input::GetInstance()->GetIsPadConnect()) return;
	
	rotation.y += Input::GetInstance()->PadRStick().x * speed_;
	rotation.x -= Input::GetInstance()->PadRStick().y * speed_;

	//上限
	rotation.x = max(rotation.x*(180.f/3.141592f), RotMinMax.x) * (3.141592f/180.f);
	rotation.x = min(rotation.x*(180.f/3.141592f), RotMinMax.y) * (3.141592f/180.f);

	//回転行列
	Matrix4x4 matRot;
	matRot = MakeIdentityMatrix();
	//matRot *= MakeRotationZMatrix(lRot.z);
	matRot *= MakeRotationXMatrix(rotation.x);
	matRot *= MakeRotationYMatrix(rotation.y);

	offset_ = Multiplication(offset_, matRot);
}

void FollowCamera::Move()
{
	translate = target_ + offset_;
}

void FollowCamera::ApplyGlobalVariablesInitialize()
{
#ifdef _DEBUG
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* name = "Camera";
	//グループを追加
	GlobalVariables::GetInstance()->CreateGroup(name);
	globalVariables->AddItem(name, "0.aspect", aspect_);
	globalVariables->AddItem(name, "1.RotSp", speed_);
	globalVariables->AddItem(name, "2.offSet", offset_);
	globalVariables->AddItem(name, "3.RotX Min,Max", RotMinMax);
#endif // _DEBUG
}

void FollowCamera::ApplyGlobalVariablesUpdate()
{
#ifdef _DEBUG
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* name = "Camera";
	ImGui::Text("Pos X: %f, Y: %f, Z:%f", translate.x,translate.y,translate.z);
	ImGui::Text("Rot X: %f, Y: %f, Z:%f", rotation.x,rotation.y,rotation.z);
	aspect_ = globalVariables->GetFloatValue(name, "0.aspect");
	speed_ = globalVariables->GetFloatValue(name, "1.RotSp");
	offset_ = globalVariables->GetVector3Value(name, "2.offSet");
	RotMinMax = globalVariables->GetVector2Value(name, "3.RotX Min,Max");
#endif // _DEBUG
}