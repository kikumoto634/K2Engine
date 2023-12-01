#include "FrameWork.h"

#include <chrono>
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

	imgui_ = ImGuiManager::Create();
	ImGuiManager::Initialize(win_->GetHWND(), dxCommon_);
}

void FrameWork::Run()
{
	std::chrono::high_resolution_clock::time_point frameStart_;
	float fps = 0.f;
	int count = 0;

	while(win_->ProcessMessage() == 0){

		frameStart_ = std::chrono::high_resolution_clock::now();

		//更新開始
		ImGuiManager::NewFrame();
		imgui_->ShowDemo();
		ImGui::Text("FPS:%lf", fps);
		app_->Update();


		//描画前
		ImGuiManager::CreateCommand();
		dxCommon_->PreDraw();

		//GeometryCommonの描画の共通処理、パイプラインをStaticで用意

		//描画
		app_->Draw();

		//描画後
		ImGuiManager::CommandsExcute(dxCommon_->GetCommandList());
		dxCommon_->PostDraw();

		auto frameEnd = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsedSeconds = frameEnd - frameStart_;
		count++;

		if(count > 30){
			fps = static_cast<float>(1.0 / elapsedSeconds.count());
			count = 0;
		}
	}
}
