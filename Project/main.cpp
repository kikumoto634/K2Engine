#include "WindowsApp.h"
#include "DirectXCommon.h"

#include <Object3D/Object3D.h>
#include "Camera.h"
#include "Geometry/Sphere.h"
#include "Geometry/Line.h"
#include "Geometry/ObjModel.h"
#include "Geometry/Sprite2D.h"

#include "Engine/Tool/ImGui/ImGuiManager.h"

//Windowsアプロでのエントリーポイント
int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int){
	
	WindowsApp* win = WindowsApp::Create(L"K2Engine", 1280, 720);
	DirectXCommon* dxCommon = DirectXCommon::Create();

	ImGuiManager* imgui = ImGuiManager::Create();
	ImGuiManager::Initialize(win->GetHWND(), dxCommon);


	Camera*camera = Camera::Create();
	//Object3D* obj = Object3D::Create();
	//Line* obj = Line::Create();
	//Sphere* obj = Sphere::Create();
	//ObjModel* obj = ObjModel::Create("cube");
	Sprite2D* obj = Sprite2D::Create();

	while(win->ProcessMessage() == 0){

		//更新開始
		ImGuiManager::NewFrame();
		imgui->ShowDemo();
		camera->Update();
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
	delete obj;
	delete camera;
	delete imgui;
	DirectXCommon::Finalize();
	WindowsApp::Finalize();

	return 0;
}