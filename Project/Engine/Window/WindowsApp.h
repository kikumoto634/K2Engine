#pragma once
#include <Windows.h>

#include <cstdint>
#include <string>
#include <format>

class WindowsApp
{
public:
	//ウィンドウプロシージャ
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	static WindowsApp* GetInstance();
	static WindowsApp* Create(wchar_t* titleName = L"TitleName", int32_t width = 1280, int32_t height = 720);

public:
	~WindowsApp(){
		CloseWindow(hwnd_);
	}
	bool ProcessMessage();

	//出力ウィンドウログ
	void Log(const std::string& message){
		OutputDebugStringA(message.c_str());
	}

	void Log_f(const char* message, ...){
		
		va_list args;
		va_start(args, message);
		int w = vsnprintf(buffer, 256-1, message, args);
		Log(buffer);
		va_end(args);
	}

	//Getter
	inline const HWND GetHWND()	{return hwnd_;}

private:
	void Initialize();



public:
	//クライアント領域
	static int32_t kWindowWidth_;
	static int32_t kWindowHeight_;

private:
	//インスタンス
	static WindowsApp* instance_;
	static wchar_t* titleName_;
	static const int kBufferSize = 256;		//書式つき文字列展開用バッファサイズ

private:
	WNDCLASS wc_{};
	RECT wrc_;
	HWND hwnd_ = nullptr;

	MSG msg;

	// 書式付き文字列展開用バッファ
	char buffer[kBufferSize] = {};
};

