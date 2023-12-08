#include "FollowCamera.h"
#include "Input.h"
#include "MathUtility.h"
#include "WindowsApp.h"
#include "GlobalVariables.h"

FollowCamera *FollowCamera::Create(Transform transform)
{
	FollowCamera* instance = new FollowCamera(transform);
	return instance;
}

FollowCamera::FollowCamera(Transform transform)
{
	translate = transform.translate;
	rotation = transform.rotation;

	//ApplyGlobalVariablesInitialize();
}

void FollowCamera::Update(Vector3 target, Vector3 offset)
{
	target_ = target;
	offset_ = offset;


	Rot();
	translate = target_ + offset_;
	//Move();

	//ApplyGlobalVariablesUpdate();
}

Matrix4x4 FollowCamera::GetViewMatrix()
{
	viewMatrix = viewMatrix.Inverse(GetWorldMatrix());
	return viewMatrix;
}

Matrix4x4 FollowCamera::GetProjectionMatrix()
{
	projectionMatrix = 
		MakePerspectiveFovMatrix(
			aspect_,
			(float)WindowsApp::kWindowWidth_/(float)WindowsApp::kWindowHeight_,
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

void FollowCamera::Move()
{
	/*Vector3 cameraPosXZ = translate;
	float height = cameraPosXZ.y;
	cameraPosXZ.y = 0.0f;
	float cameraPosXZLen = cameraPosXZ.length();
	cameraPosXZ.normalize();

	Vector3 ltarget = target_;
	ltarget.y += 0.0f;

	Vector3 newCameraPos = translate - ltarget;
	newCameraPos.y = 50.0f;
	newCameraPos.normalize();

	float weight = 0.0f;
	newCameraPos = newCameraPos * weight + cameraPosXZ * (1.0f - weight);
	newCameraPos.normalize();
	newCameraPos *= cameraPosXZLen;
	newCameraPos.y = height;
	Vector3 pos = ltarget + newCameraPos;

	translate = (pos + offset_);*/

	translate = offset_;
}

void FollowCamera::Rot()
{
	if(!Input::GetInstance()->GetIsPadConnect()) return;
	
	rotation.y += Input::GetInstance()->PadRStick().x * speed_;
	//rotation.x -= Input::GetInstance()->PadRStick().y * speed_;

	//回転行列
	Matrix4x4 matRot;
	matRot = MakeIdentityMatrix();
	matRot *= MakeRotationZMatrix(rotation.z);
	matRot *= MakeRotationXMatrix(rotation.x);
	matRot *= MakeRotationYMatrix(rotation.y);

	offset_ = Multiplication(offset_, matRot);
}

void FollowCamera::ApplyGlobalVariablesInitialize()
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
	globalVariables->AddItem(name, "4.RotSp", speed_);
	globalVariables->AddItem(name, "5.offSet", offset_);
#endif // _DEBUG
}

void FollowCamera::ApplyGlobalVariablesUpdate()
{
#ifdef _DEBUG
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* name = "Camera";
	translate = globalVariables->GetVector3Value(name, "0.translate");
	rotation = globalVariables->GetVector3Value(name, "1.rotate");
	scale = globalVariables->GetVector3Value(name, "2.scale");
	aspect_ = globalVariables->GetFloatValue(name, "3.aspect");
	speed_ = globalVariables->GetFloatValue(name, "4.RotSp");
	offset_ = globalVariables->GetVector3Value(name, "5.offSet");
#endif // _DEBUG
}
