#pragma once
#define DIRECTINPUT_VERSION 0x0800	//DirectInputのバージョン指定
#include <dinput.h>

class WindowsApp;
class KeyboardInput
{
public:
	static KeyboardInput* GetInstance();
	static void Finalize();

	void Initialize(IDirectInput8* directInput);
	void Update();

private:
	KeyboardInput() = default;
	~KeyboardInput() = default;


public:
	//入力関数
	bool Push(int key);
	bool Trigger(int key);
	bool Release(int key);

public:
	KeyboardInput(const KeyboardInput& obj) = delete;
	KeyboardInput operator=(const KeyboardInput& obj) = delete;

private:
	static KeyboardInput* instance_;
	//キー種
	static const int KeyNum_ = 256;

private:
	IDirectInputDevice8* keyboard_ = nullptr;

	BYTE key_[KeyNum_] = {};
	BYTE preKey_[KeyNum_] = {};
};

