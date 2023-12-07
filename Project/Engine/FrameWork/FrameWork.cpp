#include "FrameWork.h"
#include <imgui.h>

#include "../../GlobalVariables.h"

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

	input_ = Input::GetInstance();

	//グルーバル変数の読み込み
	GlobalVariables::GetInstance()->LoadFiles();

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

		GlobalVariables::GetInstance()->Update();

#endif // _DEBUG

		input_->Update();

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
