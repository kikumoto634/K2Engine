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
	GeometryBase::StaticInitialize();

	//obj = Object3D::Create();

	obj = Sphere::Create({{0,0,15},{0,0,0},{1,1,1}});
	obj2 = ObjModel::Create("cube", {{0,-2,15},{0,0,0},{5,0.5,5}});
	//obj = Line::Create();
	tex = Sprite2D::Create({{600,0,0},{0,0,0},{1,1,1}}, "texture.png");

	postEffect = PostEffect::Create();
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
	obj2->Update();

	tex->Update();
}



void Application::Draw()
{
	//ジオメトリ
	GeometryBase::StaticDraw();
	obj2->Draw(camera_->GetViewProjectionMatrix());

	//自作
	obj->Draw(camera_->GetViewProjectionMatrix());


	//2D
	tex->Draw(camera_->GetViewProjectionMatrix());

	postEffect->Draw(camera_->GetViewProjectionMatrix());
}