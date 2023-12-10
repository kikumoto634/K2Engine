#include "Application.h"

#include "WindowsApp.h"
#include <imgui.h>

Application *Application::Create()
{
	Application* instance = new Application();
	instance->Initialize();
	return instance;
}

void Application::Initialize()
{
	camera_ = FollowCamera::Create();
	light_ = LightingGroup::Create();

	SpriteLoader::LoadTexture(DirectXCommon::GetInstance());


	//シーンオブジェクト
	levelLoader_ = std::make_unique<LevelLoader>();
	levelLoader_->Load("levelSample");
	levelLoader_->Initialize();

	player = std::make_unique<Player>("axis");
	player.get()->translate = {0,5,0};

	sphere = Sphere::Create();
	//sphere->scale = {0.5f,0.5f,0.5f};
}

void Application::Update()
{
	//シーンオブジェクト
	levelLoader_->Update();
	player->Update();

	sphere->Update();

	camera_->Update(player->translate);
	light_->Update();
}

void Application::Draw()
{
	//シーンオブジェクト
	levelLoader_->Draw(camera_->GetViewProjectionMatrix());
	player->Draw(camera_->GetViewProjectionMatrix());
	sphere->Draw(camera_->GetViewProjectionMatrix());
}