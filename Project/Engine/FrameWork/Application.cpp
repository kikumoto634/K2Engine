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

	////シーンオブジェクト
}

void Application::Update()
{

}

void Application::GeometryDraw()
{
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
