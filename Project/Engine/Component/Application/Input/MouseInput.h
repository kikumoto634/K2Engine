#pragma once
#define DIRECTINPUT_VERSION 0x0800	//DirectInputのバージョン指定
#define MOUSE_ON_VALUE 0x80

#include <dinput.h>
#include "Vector2.h"

class WindowsApp;
class MouseInput
{
public:
	static MouseInput* GetInstance();
	static void Finalize();

	void Initialize(IDirectInput8* directInput);
	void Update();

private:
	MouseInput() = default;
	~MouseInput() = default;


public:
	//入力
	enum MouseKey{
		DIK_MOUSE_LEFT,
		DIK_MOUSE_RIGHT,
		DIK_MOUSE_WHEEL
	};

	bool Push(int key);
	bool Trigger(int key);
	bool Release(int key);

	const Vector2 ScreenPosition();
	const Vector2 MouseVelocity();

	const float WheelValue();

public:
	MouseInput(const MouseInput& obj) = delete;
	MouseInput operator=(const MouseInput& obj) = delete;

private:
	static MouseInput* instance_;

private:
	IDirectInputDevice8* mouse_ = nullptr;

	DIMOUSESTATE key_;
	DIMOUSESTATE preKey_;

	Vector2 position_;
};

