#pragma once
#define DIRECTINPUT_VERSION 0x0800	//DirectInputのバージョン指定
#include <dinput.h>
#include <Xinput.h>

#include "Vector2.h"

class WindowsApp;
class GamePadInput
{
public:
	static GamePadInput* GetInstance();
	static void Finalize();

	void Initialize();
	void Update();

	//Getter
	bool GetIsConnect()	{return isConnect_;}

private:
	GamePadInput() = default;
	~GamePadInput() = default;

public:
	//ボタン入力
	bool ButtonPush(int key);
	bool ButtonTrigger(int key);
	bool ButtonRelease(int key);

	//左右トリガー
	const int LeftTrigger();
	const bool LeftTrigger(const int value);
	const int RightTrigger();
	const bool RightTrigger(const int value);

	//スティック
	void StickUpdate();
	const Vector2 LeftStick();
	const Vector2 RightStick();

	//バイブレーション
	void VibrationUpdate();
	void VibrationStart();
	void VibrationStop();
	void VibrationLerp(const float second);


public:
	GamePadInput(const GamePadInput& obj) = delete;
	GamePadInput operator=(const GamePadInput& obj) = delete;

private:
	static GamePadInput* instance_;

	//スティック最大値
	const float StickMaxValue_ = 24079;
	//バイブレーション最大値
	const int VibrationMaxValue_ = 65535;
private:
	//Xbox 360基準
	XINPUT_STATE state_;
	XINPUT_STATE preState_;

	//接続
	bool isConnect_ = false;

	//バイブレーション
	XINPUT_VIBRATION vibration_;
	Vector2 vibrationValue_ = {};
	bool isVibrationLeap_ = false;

	float vibrationTime_ = 0;
	float vibrationTimeMax_ = 0;
};

