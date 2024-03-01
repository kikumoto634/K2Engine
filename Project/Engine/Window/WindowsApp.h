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
	static void Finalize()	{delete instance_;}

	//出力ウィンドウログ
	static void Log(const std::string& message);
	static void Log_f(double value);

	static std::wstring ConvertString(const std::string& str);
	static std::string ConvertString(const std::wstring& str);

public:
	~WindowsApp(){
		CloseWindow(hwnd_);
		CoUninitialize();
	}
	void Initialize();
	bool ProcessMessage();

	//Getter
	inline const HWND GetHWND()	{return hwnd_;}


private:
	//インスタンス
	static WindowsApp* instance_;
	static wchar_t* titleName_;
	static const int kBufferSize_ = 256;		//書式つき文字列展開用バッファサイズ

public:
	//クライアント領域
	static int32_t kWindowWidth_;
	static int32_t kWindowHeight_;

	// 書式付き文字列展開用バッファ
	static char* buffer_;

private:
	WNDCLASS wc_{};
	RECT wrc_{};
	HWND hwnd_ = nullptr;

	MSG msg_{};
};

