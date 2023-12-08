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
	levelLoader_->Load("SampleLevels");
	levelLoader_->Initialize();

	player = std::make_unique<Player>("necromancer");
}

void Application::Update()
{
	camera_->Update(player->translate, {0,3,-15});
	light_->Update();

	//シーンオブジェクト
	levelLoader_->Update();
	player->Update();
}

void Application::Draw()
{
	//シーンオブジェクト
	levelLoader_->Draw(camera_->GetViewProjectionMatrix());
	player->Draw(camera_->GetViewProjectionMatrix());
}