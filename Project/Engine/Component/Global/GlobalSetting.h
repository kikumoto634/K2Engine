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


private:
	static GlobalSetting* instance_;

private:
	const int isEnpty_ = 0b0000;
	const int isTextureSetting_ = 0b0001;

private:
	int isSettingFlag_ = 0b0000;
};

