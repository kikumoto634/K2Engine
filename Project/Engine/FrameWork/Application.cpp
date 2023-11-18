#include "Application.h"

Application *Application::Create()
{
	Application* instance = new Application();
	instance->Initialize();
	return instance;
}

void Application::Initialize()
{
	camera_ = Camera::Create();

	//obj = Object3D::Create();

	//obj = Sphere::Create();
	//obj = ObjModel::Create("cube");
	//obj = Line::Create();
	//obj = Sprite2D::Create();
}


void Application::Update()
{
	camera_->Update();

	//obj->Update();
}

void Application::Draw()
{
	//obj->Draw(camera_->GetViewProjectionMatrix());
}