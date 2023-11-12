#include "WindowsApp.h"
#include "DirectXCommon.h"

#include "Transform.h"

#include <Object3D/Object3D.h>
#include "Camera.h"

#include "Engine/Tool/ImGui/ImGuiManager.h"

//Windowsアプロでのエントリーポイント
int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int){
	
	WindowsApp* win = WindowsApp::Create(L"K2Engine", 1280, 720);
	DirectXCommon* dxCommon = DirectXCommon::Create();

	ImGuiManager* imgui = ImGuiManager::Create();
	ImGuiManager::Initialize(win->GetHWND(), dxCommon);


	Object3D* obj = Object3D::Create();
	Camera*camera = Camera::Create();

	while(win->ProcessMessage() == 0){

		//更新開始
		ImGuiManager::NewFrame();
		imgui->ShowDemo();
		obj->Update();


		//描画前
		ImGuiManager::CreateCommand();
		dxCommon->PreDraw();

		//描画
		obj->Draw(camera->GetViewProjectionMatrix());

		//描画後
		ImGuiManager::CommandsExcute(dxCommon->GetCommandList());
		dxCommon->PostDraw();
	}

	delete camera;
	delete obj;
	delete imgui;
	DirectXCommon::Finalize();
	WindowsApp::Finalize();

	return 0;
}