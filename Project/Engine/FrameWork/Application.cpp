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

	SpriteLoader::LoadTexture(DirectXCommon::GetInstance());;
	
	int Num = NumX*NumY*NumZ;
	//model_.resize(Num);
	int index = 0;

	/*for(int j = 0; j < NumZ; j++){
		for(int i = 0; i < NumY; i++){
			for(int k = 0; k < NumX; k++){

				Vector3 pos = {(float)-5+(k*1), (float)-5+(i*1), (float)(j*1)};

				ObjModel* temp = ObjModel::Create("cube", {pos,{0,0,0},{0.2f,0.2f,0.2f}}, BlendSetting::kBlendModeNone);
				model_[index++] = temp;
			}
		}
	}*/

	//testModel_= TestObj::Create("cube");

	move = ObjModel::Create("cube", {{0,0,-5.f},{},{0.5f,0.5f,0.5f}});
}

void Application::Update()
{
	//シーンオブジェクト
	/*ImGui::Text("ObjNum : %d", NumX*NumY*NumZ);*/

	/*for(auto it = model_.begin(); it != model_.end(); ++it) {
		(*it)->Update();
	}*/
	//testModel_->Update();


	move->translate = {cosf(time_), sinf(time_), -5};
	move->Update();
	time_ += 0.1f;

	camera_->Update({0,0,0});
	light_->Update();
}

void Application::GeometryDraw()
{
	/*GeometryCommon::GetInstance()->Draw();
	for(auto it = model_.begin(); it != model_.end(); ++it) {
		(*it)->Draw(camera_);
	}*/

	//TestCommon::GetInstance()->Draw();
	//testModel_->Draw(camera_);

	GeometryCommon::GetInstance()->Draw();
	move->Draw(camera_);
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
