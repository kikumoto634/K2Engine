#include "Player.h"
#include "Input.h"

#include "FollowCamera.h"
#include <MathUtility.h>
#include "Easing.h"
#include "BlendSetting.h"

#include <imgui.h>

Player::Player(std::string filePath, Transform transform):
	ObjModel(filePath, transform, BlendSetting::kBlendModeNormal)
{
	ObjModelLoad();
	Initialize(false);
	ObjModelVertexData();

	weapon_ = ObjModel::Create("weapon");

	//particle_ = ParticleObject::Create();

	name_ = "Player";
}

void Player::Update()
{
#ifdef _DEBUG
	ImGui::Text("Player - Pos X: %f, Y: %f, Z:%f", translate.x,translate.y,translate.z);
	ImGui::Text("Player - Rot X: %f, Y: %f, Z:%f", rotation.x,rotation.y,rotation.z);
	ImGui::ColorEdit4("Player - Color", &color_.x);
#endif // _DEBUG

	//状態遷移
	if(behaviorRequest_){
		behavior_ = behaviorRequest_.value();

		switch(behavior_){
		case Behavior::kRoot:
		default:
			BehaviorRootInitialize();
			break;

		case Behavior::kAttack:
			BehaviorAttackInitialize();
			break;

		case Behavior::Jump:
			BehaviorJumpInitialize();
			break;
		}
		behaviorRequest_ = std::nullopt;
	}

	//状態更新
	switch (behavior_)
	{
	case Player::Behavior::kRoot:
	default:
		BehaviorRootUpdate();
		break;

	case Player::Behavior::kAttack:
		BehaviorAttackUpdate();
		break;

	case Player::Behavior::Jump:
		BehaviorJumpUpdate();
		break;
	}

	//particle_->Update();
}

void Player::Draw(Camera* camera)
{
	switch (behavior_)
	{
	case Player::Behavior::kAttack:
		weapon_->Draw(camera);
		break;
	}

	//particle_->Draw(camera);
	ObjModel::Draw(camera);
}

void Player::OnCollision()
{
	WindowsApp::Log("Hit\n");
	//particle_->Add();
}


void Player::BehaviorRootInitialize()
{
	velocity_ = {};
}
void Player::BehaviorRootUpdate()
{
	Input();

	Move();
}

void Player::BehaviorAttackInitialize()
{
	weaponAngle_ = 0.0f;
	weaponAnimFrame_ = 0.0f;
}
void Player::BehaviorAttackUpdate()
{
	Attack();
}

void Player::BehaviorJumpInitialize()
{
	velocity_.y = kJumpVelocity;
}
void Player::BehaviorJumpUpdate()
{
	Jump();
}


void Player::Input()
{
	if(Input::GetInstance()->GetIsPadConnect()){
		if(Input::GetInstance()->PadButtonTrigger(XINPUT_GAMEPAD_A)){
			behaviorRequest_ = Behavior::Jump;
		}
		else if(Input::GetInstance()->PadButtonTrigger(XINPUT_GAMEPAD_B)){
			behaviorRequest_ = Behavior::kAttack;
		}
	}
}

void Player::Move()
{
	if(!Input::GetInstance()->GetIsPadConnect()) return;

	const float threshold = 0.7f;
	bool isMoving = false;

	velocity_ = {
		Input::GetInstance()->PadLStick().x,0.0f,Input::GetInstance()->PadLStick().y
	};
	velocity_ = velocity_.normalize();

	if(velocity_.length() > threshold){
		isMoving = true;
	}

	if(!isMoving) return;

	velocity_ *= kMoveVelocity;

	//カメラの方向へと動く
	Matrix4x4 matRot;
	matRot = MakeIdentityMatrix();
	//matRot *= MakeRotationZMatrix(FollowCamera::GetInstance()->rotation.z);
	//matRot *= MakeRotationXMatrix(FollowCamera::GetInstance()->rotation.x);
	matRot *= MakeRotationYMatrix(FollowCamera::GetInstance()->rotation.y);
	velocity_ = Multiplication(velocity_, matRot);

	//回転
	rotation.y = std::atan2(velocity_.x, velocity_.z);
	//Vector3 velocityXZ = Vector3{move.x, 0, move.z};
	//rotation.x = std::atan2(-move.y, velocityXZ.length());

	//移動
	translate += velocity_;
}

void Player::Attack()
{
	weaponAngle_ = Easing_Point2_EaseOutBounce(0.0f, 1.8f, Time_OneWay(weaponAnimFrame_, 1.0f));

	weapon_->rotation.x = weaponAngle_;
	weapon_->rotation.y = rotation.y;
	weapon_->translate = translate;

	if(weaponAnimFrame_ >= 1.f){
		behaviorRequest_ = Behavior::kRoot;
	}
}

void Player::Jump()
{
	translate += velocity_;
	Vector3 accelerationVector = {0, -kGravityAcceleration, 0};
	velocity_ += accelerationVector;

	if(translate.y <= 0.0f){
		translate.y = 0.0f;
		behaviorRequest_ = Behavior::kRoot;
	}
}
