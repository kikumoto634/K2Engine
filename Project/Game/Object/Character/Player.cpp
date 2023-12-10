#include "Player.h"
#include "Input.h"

#include "FollowCamera.h"
#include <MathUtility.h>

#include <imgui.h>

Player::Player(std::string filePath, Transform transform):
	ObjModel(filePath, transform)
{
	ObjModelLoad();
	Initialize(false);
	ObjModelVertexData();

	name = "Player";
}

void Player::Update()
{
#ifdef _DEBUG
	ImGui::Text("Player - Pos X: %f, Y: %f, Z:%f", translate.x,translate.y,translate.z);
	ImGui::Text("Player - Rot X: %f, Y: %f, Z:%f", rotation.x,rotation.y,rotation.z);
#endif // _DEBUG
	if(!Input::GetInstance()->GetIsPadConnect()) return;

	Vector3 move = {
		Input::GetInstance()->PadLStick().x,0.0f,Input::GetInstance()->PadLStick().y
	};
	move = move.normalize() * 1.0f;

	//カメラの方向へと動く
	Matrix4x4 matRot;
	matRot = MakeIdentityMatrix();
	matRot *= MakeRotationZMatrix(FollowCamera::GetInstance()->rotation.z);
	matRot *= MakeRotationXMatrix(FollowCamera::GetInstance()->rotation.x);
	matRot *= MakeRotationYMatrix(FollowCamera::GetInstance()->rotation.y);
	move = Multiplication(move, matRot);

	if(move == Vector3{0,0,0}) return;

	//回転
	/*rotation.y = std::atan2(move.x, move.z);
	Vector3 velocityXZ = Vector3{move.x, 0, move.z};
	rotation.x = std::atan2(-move.y, velocityXZ.length());*/

	//移動
	translate +=  move;
}
