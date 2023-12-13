#include "Player.h"
#include "Input.h"

#include "FollowCamera.h"
#include <MathUtility.h>

#include <imgui.h>

Player::Player(std::string filePath, Transform transform):
	ObjModel(filePath, transform)
{
	//color_ = {0,0,0,1};

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
	if(Input::GetInstance()->GetIsPadConnect()){

		const float threshold = 0.7f;
		bool isMoving = false;

		Vector3 move = {
			Input::GetInstance()->PadLStick().x,0.0f,Input::GetInstance()->PadLStick().y
		};
		if(move.length() > threshold){
			isMoving = true;
		}

		if(isMoving){
			//移動
			translate +=  move;

			//カメラの方向へと動く
			Matrix4x4 matRot;
			matRot = MakeIdentityMatrix();
			matRot *= MakeRotationZMatrix(FollowCamera::GetInstance()->rotation.z);
			matRot *= MakeRotationXMatrix(FollowCamera::GetInstance()->rotation.x);
			matRot *= MakeRotationYMatrix(FollowCamera::GetInstance()->rotation.y);
			move = Multiplication(move, matRot);

			//回転
			//目標の角度補間
			//angle = std::atan2(move.x, move.z);
		}
		/*else{
			if(rotation.y > 360.f*(3.141592f/180.f)){
			rotation.y -= 360.f*(3.141592f/180.f);
			}
			else if(rotation.y < -360.f*(3.141592f/180.f)){
				rotation.y += 360.f*(3.141592f/180.f);
			}
		}*/
	}
	//rotation.y = LerpShortAngle(rotation.y, angle, 0.1f);
}