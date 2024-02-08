#include "FrameWork.h"
#include <imgui.h>

#include "GlobalVariables.h"
#include "GlobalSetting.h"

FrameWork *FrameWork::Create()
{
	FrameWork* instance = new FrameWork();
	instance->Initialize();
	return instance;
}

void FrameWork::Initialize()
{
	win_ = WindowsApp::Create(L"K2Engine", 1280, 720);
	dxCommon_ = DirectXCommon::Create({0.25,0.5,0.25,1});

	input_ = Input::GetInstance();


	//グルーバル変数の読み込み
	GlobalVariables::GetInstance()->LoadFiles();
	imgui_ = ImGuiManager::Create();
	ImGuiManager::Initialize(win_->GetHWND(), dxCommon_);

	SpriteCommon::GetInstance();
	GeometryCommon::GetInstance();
	ParticleCommon::GetInstance();
	GPUParticleCommon::GetInstance();

	app_ = Application::Create();

}

void FrameWork::Run()
{
	while(win_->ProcessMessage() == 0){

		//更新開始
		ImGuiManager::NewFrame();
#ifdef _DEBUG
		imgui_->ShowDemo();

		GlobalSetting::GetInstance()->Update();

#endif // _DEBUG
		GlobalVariables::GetInstance()->Update();

		input_->Update();

		app_->Update();


#ifdef _DEBUG

		ImGuiManager::CreateCommand();
#endif // _DEBUG
		dxCommon_->PreDraw();

		//描画
		app_->GeometryDraw();
		app_->SpriteDraw();
		app_->ParticleDraw();

#ifdef _DEBUG
		ImGuiManager::CommandsExcute(dxCommon_->GetCommandList());
#endif // _DEBUG
		dxCommon_->PostDraw();
	}
}
