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

	SpriteLoader::LoadTexture(DirectXCommon::GetInstance());

	//obj = Object3D::Create();

	obj = Sphere::Create();
	//obj = ObjModel::Create("cube");
	//obj = Line::Create();
	//obj = Sprite2D::Create();
}

void Application::Update()
{
	camera_->Update();

	for(auto tex : SpriteLoader::GetTexture()){
		if(tex.filePath == "") break;
		ImGui::Text("%d: %s", tex.index, tex.filePath.c_str());
	}

	obj->Update();
}

void Application::Draw()
{
	obj->Draw(camera_->GetViewProjectionMatrix());
}