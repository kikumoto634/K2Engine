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

	player_ = std::make_unique<Player>("cube");
	player_.get()->translate = {0,0,0};
	player_.get()->scale = {0.8f,0.8f,0.8f};

	box_ = ObjModel::Create("fence", {{0,0,0},{0,0,0},{1,1,1}}, BlendSetting::kBlendModeNone);
	box_->translate = {-5.f,0,0};
	box_->rotation = DegreesToRadians({0,180.f,0});
	box_->scale = {0.8f,0.8f,0.8f};


	collisionManager_ = std::make_unique<CollisionManager>();

	particle_ = ParticleObject::Create();
	//sp_ = Sprite2D::Create();
}

void Application::Update()
{
	//シーンオブジェクト
	levelLoader_->Update();
	player_->Update();


	box_->Update();
	particle_->Update();
	//sp_->Update();

	camera_->Update(player_->translate);
	light_->Update();

	CollisionCheck();
}

void Application::GeometryDraw()
{
	//シーンオブジェクト
	levelLoader_->Draw(camera_->GetViewProjectionMatrix());
	player_->Draw(camera_->GetViewProjectionMatrix());
	box_->Draw(camera_->GetViewProjectionMatrix());

	collisionManager_->Draw(camera_->GetViewProjectionMatrix());

	//sp_->Draw(camera_->GetViewMatrix());
}

void Application::ParticleDraw()
{
	particle_->Draw(camera_->GetViewProjectionMatrix());
}

void Application::CollisionCheck()
{
	collisionManager_->Reset();

	collisionManager_->AddCollider(player_.get());
	collisionManager_->AddCollider(box_);

	collisionManager_->Update();
	collisionManager_->CheckAllCollisions();
}
