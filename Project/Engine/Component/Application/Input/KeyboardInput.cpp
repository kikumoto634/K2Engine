#include "KeyboardInput.h"
#include <cassert>

#include "WindowsApp.h"

KeyboardInput* KeyboardInput::instance_ = nullptr;

KeyboardInput *KeyboardInput::GetInstance()
{
	if(!instance_){
		instance_ = new KeyboardInput();
	}
	return instance_;
}

void KeyboardInput::Finalize()
{
	if(instance_){
		delete instance_;
		instance_ = nullptr;
	}
}


void KeyboardInput::Initialize(IDirectInput8* directInput)
{
	HRESULT result{};
	assert(directInput);

	result = directInput->CreateDevice(
		GUID_SysKeyboard,
		&keyboard_,
		NULL
	);
	assert(SUCCEEDED(result));

	result = keyboard_->SetDataFormat(&c_dfDIKeyboard);	//標準形式
	assert(SUCCEEDED(result));

	//排他的制御レベルのセット
	//DISCL_FOREGROUND		画面が手前にある場合のみ入力を受け付ける
	//DISCL_NONEXCLUSIVE	デバイスをこのアプリだけで専有しない
	//DISCL_NOWINKEY		Windowsキーを無効にする
	result = keyboard_->SetCooperativeLevel(
		WindowsApp::GetInstance()->GetHWND(),
		DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY
	);
	assert(SUCCEEDED(result));
}

void KeyboardInput::Update()
{
	HRESULT result{};
	result = keyboard_->Acquire();
	assert(SUCCEEDED(result));

	for(int i = 0; i < KeyNum_; i++){
		preKey_[i] = key_[i];
	}

	result = keyboard_->GetDeviceState(sizeof(key_), key_);
	assert(SUCCEEDED(result));
}


bool KeyboardInput::Push(int key)
{
	if(key < 0x00) return false;
	if(key > 0xff) return false;

	if(key_[key])
	{
		return true;
	}
	return false;
}

bool KeyboardInput::Trigger(int key)
{
	if(key < 0x00) return false;
	if(key > 0xff) return false;

	if(key_[key] && !preKey_[key])
	{
		return true;
	}
	return false;
}

bool KeyboardInput::Release(int key)
{
	if(key < 0x00) return false;
	if(key > 0xff) return false;

	if(!key_[key] && preKey_[key])
	{
		return true;
	}
	return false;
}
