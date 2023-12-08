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
	if(Input::GetInstance()->Push(DIK_D)){
		translate.x += 1.f;
	}
	else if(Input::GetInstance()->Push(DIK_A)){
		translate.x -= 1.f;
	}
}
