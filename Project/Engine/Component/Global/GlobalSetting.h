#pragma once
#include <string>

class GlobalSetting
{
public:
	static GlobalSetting* GetInstance();

private:
	GlobalSetting() = default;
	~GlobalSetting() = default;
	GlobalSetting(const GlobalSetting& other) = default;
	GlobalSetting& operator=(const GlobalSetting& other) = default;

public:
	void Update();


private:
	//メニューボタン
	void MenuBotton(std::string name, int flag);

	//テクスチャ表示
	void LoadTextureDraw();
	//FPS表示
	void FpsDraw();
	//入力関連
	void InputDraw();

private:
	static GlobalSetting* instance_;

private:
	const int isEnpty_ = 0b0000;
	//画像
	const int isTextureSetting_ = 0b0001;
	//FPS
	const int isFpsSetting = 0b0010;
	//入力
	const int isInputSetting = 0b0100;

private:
	int isSettingFlag_ = 0b0000;
};

