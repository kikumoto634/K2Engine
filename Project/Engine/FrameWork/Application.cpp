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

	player = std::make_unique<Player>("cube");
	player.get()->translate = {0,5,0};

	box = ObjModel::Create("cube");
	box->translate = {-2.1f,5,0};
	box->scale = {0.5f,0.5f,0.5f};

	collisionManager_ = std::make_unique<CollisionManager>();
}

void Application::Update()
{
	//シーンオブジェクト
	levelLoader_->Update();
	player->Update();


	box->Update();

	camera_->Update(player->translate);
	light_->Update();

	CollisionCheck();
}

void Application::Draw()
{
	//シーンオブジェクト
	levelLoader_->Draw(camera_->GetViewProjectionMatrix());
	player->Draw(camera_->GetViewProjectionMatrix());
	box->Draw(camera_->GetViewProjectionMatrix());
}

void Application::CollisionCheck()
{
	collisionManager_->Reset();

	collisionManager_->AddCollider(player.get());
	collisionManager_->AddCollider(box);

	collisionManager_->CheckAllCollisions();
}
