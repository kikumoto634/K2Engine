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
	camera_ = Camera::Create();
	light_ = LightingGroup::Create();

	SpriteLoader::LoadTexture(DirectXCommon::GetInstance());

	//obj = Object3D::Create();

	//obj = Sphere::Create({{0,0,15},{0,0,0},{1,1,1}});
	obj2 = ObjModel::Create("cube", {{0,-2,15},{0,0,0},{5,0.5,5}});
	//obj = Line::Create();
	obj = Sprite2D::Create();
}

void Application::Update()
{
//オブジェクトImGui座標
#ifdef _DEBUG
	ImGui::SetNextWindowPos({0,0});
	ImGui::SetNextWindowSize({400,500});
	ImGui::Begin("Object");
#endif // _DEBUGz
	camera_->Update();
	light_->Update();

	obj->Update();
	obj2->Update();
#ifdef _DEBUG
	ImGui::End();
#endif // _DEBUG
}

void Application::Draw()
{
	obj->Draw(camera_->GetViewProjectionMatrix());
	obj2->Draw(camera_->GetViewProjectionMatrix());
}