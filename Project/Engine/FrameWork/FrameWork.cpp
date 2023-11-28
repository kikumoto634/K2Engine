#include "FrameWork.h"

FrameWork *FrameWork::Create()
{
	FrameWork* instance = new FrameWork();
	instance->Initialize();
	return instance;
}

void FrameWork::Initialize()
{
	win_ = WindowsApp::Create(L"K2Engine", 1280, 720);
	dxCommon_ = DirectXCommon::Create();
	shadowCommon_ = new ShadowCommon;
	shadowCommon_->Initialize();

	app_ = Application::Create();

	imgui_ = ImGuiManager::Create();
	ImGuiManager::Initialize(win_->GetHWND(), dxCommon_);
}

void FrameWork::Run()
{
	while(win_->ProcessMessage() == 0){

		//更新開始
		ImGuiManager::NewFrame();
		imgui_->ShowDemo();
		app_->Update();


		shadowCommon_->PreDraw();

		app_->ShadowDraw();

		shadowCommon_->PostDraw();


		//描画前
		ImGuiManager::CreateCommand();
		dxCommon_->PreDraw();

		//描画
		app_->Draw();

		//描画後
		ImGuiManager::CommandsExcute(dxCommon_->GetCommandList());
		dxCommon_->PostDraw();
	}
}
