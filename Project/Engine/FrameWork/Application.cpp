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
	colliisonManager_ = CollisionManager::GetInstance();

	levelLoader_ = std::make_unique<LevelLoader>();
	levelLoader_->Load("levelSample");
	levelLoader_->Initialize();

	player = std::make_unique<Player>("cube");
	player.get()->translate = {0,5,0};
	player->SetCollisionAttribute(kPlayerCollisionAttribute);
	//player->SetCollisionMask(kPlayerCollisionAttribute);
	colliisonManager_->AddCollider(player.get());

	box = ObjModel::Create("cube");
	box->translate = {-2.1f,5,0};
	box->scale = {0.5f,0.5f,0.5f};
	box->SetCollisionMask(kPlayerCollisionAttribute);
	colliisonManager_->AddCollider(box);
}

void Application::Update()
{
	//シーンオブジェクト
	levelLoader_->Update();
	player->Update();
	player->SetCenter(player->translate);
	player->SetR(1.0f);


	box->Update();
	box->SetCenter(box->translate);
	box->SetR(1.0f);

	camera_->Update(player->translate);
	light_->Update();

	colliisonManager_->CollisionAllCheck();
}

void Application::Draw()
{
	//シーンオブジェクト
	levelLoader_->Draw(camera_->GetViewProjectionMatrix());
	player->Draw(camera_->GetViewProjectionMatrix());
	box->Draw(camera_->GetViewProjectionMatrix());
}