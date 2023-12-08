#pragma once
#include "Camera.h"
#include "LightingGroup.h"

#include "Geometry/Sphere.h"
#include "Geometry/ObjModel.h"
#include "Geometry/Sprite2D.h"


#include "SpriteLoader.h"

//シーンオブジェクト
#include "LevelLoader.h"
#include "../../Game/Object/Character/Player.h"
#include "FollowCamera.h"

class Application
{
public:
	static Application* Create();

public:
	~Application(){

		//シーンオブジェクト

	}

	void Update();
	void Draw();

private:
	void Initialize();

private:
	FollowCamera* camera_ = nullptr;
	LightingGroup* light_ = nullptr;

	//シーンオブジェクト
	std::unique_ptr<LevelLoader> levelLoader_;
	std::unique_ptr<Player> player;
};

