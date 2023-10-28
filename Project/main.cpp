#include "WindowsApp.h"
#include "DirectXCommon.h"

//Windowsアプロでのエントリーポイント
int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int){
	
	WindowsApp* win = WindowsApp::Create(L"K2Engine", 1280, 720);
	DirectXCommon* dxCommon = DirectXCommon::Create();

	while(win->ProcessMessage() == 0){
		dxCommon->Draw();
	}

	delete dxCommon;
	delete win;

	return 0;
}