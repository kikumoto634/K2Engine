#include "WindowsApp.h"
#include "DirectXCommon.h"

#include "Transform.h"

#include <Object3D/Object3D.h>
#include "Camera.h"

//Windowsアプロでのエントリーポイント
int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int){
	
	WindowsApp* win = WindowsApp::Create(L"K2Engine", 1280, 720);
	DirectXCommon* dxCommon = DirectXCommon::Create();

	Object3D* obj = Object3D::Create();
	Camera*camera = Camera::Create();

	while(win->ProcessMessage() == 0){
		dxCommon->PreDraw();
		obj->Draw(camera->GetViewProjectionMatrix());
		dxCommon->PostDraw();
	}

	delete camera;
	delete obj;
	delete dxCommon;
	delete win;

	return 0;
}