#pragma once
#include "Vector2.h"

#define DIRECTINPUT_VERSION 0x0800	//DirectInputのバージョン指定
#include <dinput.h>

class KeyboardInput;
class MouseInput;
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

private:
	static InputManager* instance_;
private:
	WindowsApp* window = nullptr;

	KeyboardInput* keyboard_ = nullptr;
	MouseInput* mouse_ = nullptr;
	GamePadInput* gamePad_ = nullptr;


	IDirectInput8* directInput_ = nullptr;
};

