#pragma once
#include "Vector2.h"

#define DIRECTINPUT_VERSION 0x0800	//DirectInputのバージョン指定
#include <dinput.h>

//マウス
#define MOUSE_ON_VALUE 0x80

//パッド
#include <Xinput.h>

class KeyboardInput;

class WindowsApp;
class InputManager
{
public:
	~InputManager();

	static InputManager* GetInstance();

	//初期化
	void Initialize();

	//更新
	void Update();

#pragma region マウス入力
	/// <summary>
	/// 0:Left, 1;Right, 2:Center
	/// </summary>
	bool MousePush(int keyNumber);
	bool MouseTrigger(int keyNumber);
	
	//マウスの座標
	const Vector2 GetMousePos();
	//マウスの移動量
	const Vector2 GetMouseVelocity();
	//マウスホイール
	const float GetMouseWheel();
#pragma endregion

#pragma region パッド
	bool GetIsPadConnect()	{return isPadConnect_;}

	//ボタン
	bool PadButtonPush(int keyNumber);
	bool PadButtonTrigger(int keyNumber);

	//LT、RT
	bool PadLeftTrigger(int Value = 30);
	bool PadRightTrigger(int Value = 30);

	//Stick Max32767
	Vector2 PadLStick();
	Vector2 PadRStick();
	Vector2 PadLStickPre();
	Vector2 PadRStickPre();

	bool PadLStickRightPush()	{return (PadLStick().x == +1);}
	bool PadLStickLeftPush()	{return (PadLStick().x == -1);}
	bool PadLStickUpPush()		{return (PadLStick().y == +1);}
	bool PadLStickDownPush()	{return (PadLStick().y == -1);}

	bool PadLStickRightTrigger(){return (PadLStickRightPush() && !(PadLStickPre().x == +1));}
	bool PadLStickLeftTrigger()	{return (PadLStickLeftPush()  && !(PadLStickPre().x == -1));}
	bool PadLStickUpTrigger()	{return (PadLStickUpPush()    && !(PadLStickPre().y == +1));}
	bool PadLStickDownTrigger()	{return (PadLStickDownPush()  && !(PadLStickPre().y == -1));}
	
	bool PadRStickRightPush()	{return (PadRStick().x == +1);}
	bool PadRStickLeftPush()	{return (PadRStick().x == -1);}
	bool PadRStickUpPush()		{return (PadRStick().y == +1);}
	bool PadRStickDownPush()	{return (PadRStick().y == -1);}

	bool PadRStickRightTrigger(){return (PadRStickRightPush() && !(PadRStickPre().x == +1));}
	bool PadRStickLeftTrigger()	{return (PadRStickLeftPush()  && !(PadRStickPre().x == -1));}
	bool PadRStickUpTrigger()	{return (PadRStickUpPush()    && !(PadRStickPre().y == +1));}
	bool PadRStickDownTrigger()	{return (PadRStickDownPush()  && !(PadRStickPre().y == -1));}

	
	//Vibration
	void PadVibrationStart();
	void PadVibrationStop();
	void PadVibrationLeap(const float Second);
#pragma endregion

private:
	static InputManager* instance_;
private:
	WindowsApp* window = nullptr;

	KeyboardInput* keyboard_ = nullptr;

	IDirectInput8* directInput_ = nullptr;

#pragma region マウス
	//マウスデバイス
	IDirectInputDevice8* mouse_ = nullptr;
	//マウス情報
	DIMOUSESTATE mouseKey_;
	DIMOUSESTATE mousePreKey_;

	//座標
	Vector2 mousePos_;
#pragma endregion

#pragma region パッド
	const float StickMaxValue_ = 24079;

	const int VibrationMaxValue_ = 65535;

	void PadUpdate();

	//Xbox 360基準
	XINPUT_STATE padState_;
	XINPUT_STATE padPreState_;

	bool isPadConnect_ = false;

	XINPUT_VIBRATION vibration_;
	bool isVibrationLeap_ = false;
	Vector2 vibrationValue_ = {};
	float vibrationTime_ = 0;
	float vibrationTimeMax_ = 0;
#pragma endregion
};

