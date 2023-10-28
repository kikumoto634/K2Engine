#include <Windows.h>

#include "WindowsApp.h"

//Windowsアプロでのエントリーポイント
int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int){
	
	WindowsApp* win = WindowsApp::Create(L"K2Engine", 1280, 720);


	win->Log("aa\n");
	win->Log_f("%d\n", 5);

	while(win->ProcessMessage() == 0){
	
	}

	delete win;

	return 0;
}