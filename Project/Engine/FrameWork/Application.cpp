#include "Application.h"

#include "WindowsApp.h"
#include <imgui.h>
#include "Geometry/SphereCollider.h"

Application *Application::Create()
{
	Application* instance = new Application();
	instance->Initialize();
	return instance;
}

void Application::Initialize()
{
	camera_ = FollowCamera::Create({{0,0,0},{0,0,0},{1,1,1}});
	light_ = LightingGroup::Create();

	SpriteLoader::LoadTexture(DirectXCommon::GetInstance());

	//シーンオブジェクト
	for(int i = 0; i < CreateNum; i++){
		for(int j = 0; j < 7; j++){
			Transform trans = {{(float)(j*32) - 100, -5, (float)(i*32)},{},{1,1,1}};
			ObjModel* obj = ObjModel::Create("cube",trans);
			obj_.push_back(obj);
		}
	}
}

void Application::Update()
{
	for(ObjModel* v : obj_){
		v->Update();
	}
}

void Application::GeometryDraw()
{
	for(ObjModel* v : obj_){
		v->Draw(camera_);
	}
}

void Application::SpriteDraw()
{
}

void Application::ParticleDraw()
{
}

void Application::CollisionCheck()
{
}
