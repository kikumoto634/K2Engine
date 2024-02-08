#pragma once
#include "Vector2.h"

#define DIRECTINPUT_VERSION 0x0800	//DirectInputのバージョン指定
#include <dinput.h>

//マウス
#define MOUSE_ON_VALUE 0x80

class KeyboardInput;
class GamePadInput;

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

private:
	static InputManager* instance_;
private:
	WindowsApp* window = nullptr;

	KeyboardInput* keyboard_ = nullptr;
	GamePadInput* gamePad_ = nullptr;


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
};

