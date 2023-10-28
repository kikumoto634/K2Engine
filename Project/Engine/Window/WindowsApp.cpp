#include "WindowsApp.h"

int32_t WindowsApp::kWindowWidth_ = 1280;
int32_t WindowsApp::kWindowHeight_ = 720;

wchar_t* WindowsApp::titleName_ = L"Title";

WindowsApp* WindowsApp::instance_ = nullptr;

LRESULT WindowsApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//メッセージに応じてゲーム固有の処理
	switch (msg)
	{
		//ウィンドウの破棄
	case WM_DESTROY:
		//OSに対して、アプリの終了
		PostQuitMessage(0);
		return 0;
	}

	//標準のメッセージ処理
	return DefWindowProc(hwnd,msg,wparam,lparam);
}

WindowsApp *WindowsApp::GetInstance()
{
	if(!instance_){
		instance_ = new WindowsApp();
	}
	return instance_;
}

WindowsApp *WindowsApp::Create(wchar_t* titleName, int32_t width, int32_t height)
{
	kWindowWidth_ = width;
	kWindowHeight_ = height;
	titleName_ = titleName;

	if(instance_ == nullptr){
		instance_ = new WindowsApp();
		instance_->Initialize();
	}
	return instance_;
}

void WindowsApp::Initialize()
{
	//ウィンドウプロシージャ
	wc_.lpfnWndProc = WindowProc;
	//ウィンドウクラス名
	wc_.lpszClassName  = L"Windows";
	//インスタンスハンドル
	wc_.hInstance = GetModuleHandle(nullptr);
	//カーソル
	wc_.hCursor = LoadCursor(nullptr, IDC_ARROW);

	//ウィンドウクラス登録
	RegisterClass(&wc_);


	//ウィンドウサイズを表す構造体にクライアント領域
	wrc_ = {0,0,kWindowWidth_,kWindowHeight_};

	//クライアント領域をもとに実際のサイズにwrcを変更
	AdjustWindowRect(&wrc_, WS_OVERLAPPEDWINDOW, false);


	//ウィンドウ生成
	hwnd_ = CreateWindow(
		wc_.lpszClassName,			//利用するクラス
		titleName_,					//タイトルバー
		WS_OVERLAPPEDWINDOW,		//ウィンドウスタイル
		CW_USEDEFAULT,				//表示X座法
		CW_USEDEFAULT,				//表示Y座標
		wrc_.right - wrc_.left,		//ウィンドウ横幅
		wrc_.bottom - wrc_.top,		//ウィンドウ縦幅
		nullptr,					//ウィンドウハンドル
		nullptr,					//メニューハンドル
		wc_.hInstance,				//インスタンスハンドル
		nullptr						//オプション
	);

	//ウィンドウ表示
	ShowWindow(hwnd_, SW_SHOW);
}

bool WindowsApp::ProcessMessage()
{
	if(PeekMessage(&msg,nullptr,0,0,PM_REMOVE)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if(msg.message == WM_QUIT){
		return true;
	}

	return false;
}