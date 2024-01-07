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
	player_->SetCollisionAttribute(kCollisionAttributePlayer);
	player_->SetCollisionMask(kCollisionAttributeEnemy);

	box_ = ObjModel::Create("fence", {{0,0,0},{0,0,0},{1,1,1}}, BlendSetting::kBlendModeNone);
	box_->translate = {-5.f,0,0};
	box_->rotation = DegreesToRadians({0,180.f,0});
	box_->scale = {0.8f,0.8f,0.8f};
	box_->SetCollisionAttribute(kCollisionAttributeEnemy);
	box_->SetCollisionMask(kCollisionAttributePlayer);

	collisionManager_ = std::make_unique<CollisionManager>();

	particle_ = ParticleObject::Create();
	emitter_ = ParticleEmitterObject::Create();

	sp_ = Sprite2D::Create();


	sphere.center = {0,2,0};
	sphere.radius = 1.0f;

	plane.normal = {0,1,0};
	plane.distance = 0.0f;

	triangle.p0 = {-1,0,-1};
	triangle.p1 = {-1,0, 1};
	triangle.p2 = { 1,0,-1};
	triangle.normal = {0,1,0};
}

void Application::Update()
{
	//シーンオブジェクト
	levelLoader_->Update();
	player_->Update();
	box_->Update();

	sp_->Update();

	particle_->Add(particlePos_);
	particle_->Update();

	emitter_->Add(emitterPos_);
	emitter_->Update();

	camera_->Update(player_->translate);
	light_->Update();


	if(Input::GetInstance()->Push(DIK_UP))			sphere.center += {0,0.01f,0};
	else if(Input::GetInstance()->Push(DIK_DOWN))	sphere.center -= {0,0.01f,0};
	if(Input::GetInstance()->Push(DIK_RIGHT))		sphere.center += {0.01f,0,0};
	else if(Input::GetInstance()->Push(DIK_LEFT))	sphere.center -= {0.01f,0,0};
	
	ImGui::Text("\nSphere:(%f,%f,%f)", sphere.center.x,sphere.center.y,sphere.center.z);
	Vector3 inter;
	bool hit = CollisionCheck::CheckSphere2Triangle(sphere, triangle, &inter);
	if(hit){
		ImGui::Text("Hit");
		ImGui::Text("Inter:(%f,%f,%f)", inter.x,inter.y,inter.z);
	}


	CollisionCheck();
}

void Application::GeometryDraw()
{
	//シーンオブジェクト
	levelLoader_->Draw(camera_);
	player_->Draw(camera_);
	box_->Draw(camera_);

	collisionManager_->Draw(camera_);
}

void Application::SpriteDraw()
{
	sp_->Draw(camera_);
}

void Application::ParticleDraw()
{
	particle_->Draw(camera_);
	emitter_->Draw(camera_);
}

void Application::CollisionCheck()
{
	collisionManager_->Reset();

	collisionManager_->AddCollider(player_.get());
	collisionManager_->AddCollider(box_);

	collisionManager_->Update();
	collisionManager_->CheckAllCollisions();
}
