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

	 obj = Sphere::Create({{0,0,10},{0,0,0},{1,1,1}});
	//obj = ObjModel::Create("cube");
	//obj = Line::Create();
	//obj = Sprite2D::Create();
}

void Application::Update()
{
	camera_->Update();
	light_->Update();

	if(ImGui::TreeNode("LoadTexture  Index : Name")){
		for(auto tex : SpriteLoader::GetTexture()){
			if(tex.filePath == "") break;
			ImGui::Text("%d: %s", tex.index, tex.filePath.c_str());
		}
		ImGui::TreePop();
	}

	obj->Update();
}

void Application::Draw()
{
	obj->Draw(camera_->GetViewProjectionMatrix());
}