#include "FrameWork.h"
#include <imgui.h>

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

	app_ = Application::Create();

#ifdef _DEBUG
	imgui_ = ImGuiManager::Create();
	ImGuiManager::Initialize(win_->GetHWND(), dxCommon_);
#endif // _DEBUG
}

void FrameWork::Run()
{
	while(win_->ProcessMessage() == 0){

		//更新開始
#ifdef _DEBUG
		ImGuiManager::NewFrame();
		//imgui_->ShowDemo();
#endif // _DEBUG
		app_->Update();


		//描画前
#ifdef _DEBUG
		ImGuiManager::CreateCommand();
#endif // _DEBUG
		dxCommon_->PreDraw();

		//描画
		app_->Draw();

		//描画後
#ifdef _DEBUG
		ImGuiManager::CommandsExcute(dxCommon_->GetCommandList());
#endif // _DEBUG
		dxCommon_->PostDraw();
	}
}
