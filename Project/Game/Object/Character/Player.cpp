#include "Player.h"
#include "Input.h"

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
	if(!Input::GetInstance()->GetIsPadConnect()) return;

	Vector3 move = {
		Input::GetInstance()->PadLStick().x,0.0f,Input::GetInstance()->PadLStick().y
	};
	move = move.normalize() * 1.0f;

	translate +=  move;
}
