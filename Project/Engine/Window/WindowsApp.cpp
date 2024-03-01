#include "WindowsApp.h"

#include "../../External/imgui/imgui.h"

int32_t WindowsApp::kWindowWidth_ = 1280;
int32_t WindowsApp::kWindowHeight_ = 720;
wchar_t* WindowsApp::titleName_ = L"Title";
WindowsApp* WindowsApp::instance_ = nullptr;

char* WindowsApp::buffer_ = "";

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

#pragma comment(lib, "winmm.lib")

LRESULT WindowsApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if(ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)){
		return true;
	}

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
	CoInitializeEx(0, COINIT_MULTITHREADED);
	//システムタイマーの精度
	timeBeginPeriod(1);

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
	if(PeekMessage(&msg_,nullptr,0,0,PM_REMOVE)){
		TranslateMessage(&msg_);
		DispatchMessage(&msg_);
	}

	if(msg_.message == WM_QUIT){
		return true;
	}

	return false;
}


void WindowsApp::Log(const std::string &message)
{
	OutputDebugStringA(message.c_str());
}

void WindowsApp::Log_f(double value)
{
	/*va_list args;
	va_start(args, message);
	int w = vsnprintf(buffer_, kBufferSize_-1, message, args);
	Log(buffer_);
	va_end(args);*/
	Log(std::format("Fps:{}\n", value));
}

std::wstring WindowsApp::ConvertString(const std::string &str)
{
	if(str.empty()){
		return std::wstring();
	}

	auto sizeNeeded = MultiByteToWideChar(CP_UTF8,0,reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
	if(sizeNeeded == 0){
		return std::wstring();
	}

	std::wstring result(sizeNeeded,0);
	MultiByteToWideChar(CP_UTF8,0,reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
	return result;
}

std::string WindowsApp::ConvertString(const std::wstring &str)
{
	if(str.empty()){
		return std::string();
	}
	auto sizeNeeded = WideCharToMultiByte(CP_UTF8,0,str.data(), static_cast<int>(str.size()), NULL, 0, NULL,NULL);
	if(sizeNeeded == 0){
		return std::string();
	}
	std::string result(sizeNeeded,0);
	WideCharToMultiByte(CP_UTF8,0,str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL,NULL);
	return result;
}
