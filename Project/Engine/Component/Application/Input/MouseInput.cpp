#include "MouseInput.h"
#include <cassert>

#include "WindowsApp.h"

MouseInput* MouseInput::instance_ = nullptr;

MouseInput *MouseInput::GetInstance()
{
	if(!instance_){
		instance_ = new MouseInput();
	}

	return instance_;
}

void MouseInput::Finalize()
{
	if(instance_){
		delete instance_;
		instance_ = nullptr;
	}
}


void MouseInput::Initialize(IDirectInput8 *directInput)
{
	HRESULT result{};

	result = directInput->CreateDevice(
		GUID_SysMouse,
		&mouse_,
		NULL
	);
	assert(SUCCEEDED(result));

	result = mouse_->SetDataFormat(&c_dfDIMouse);
	assert(SUCCEEDED(result));

	result = mouse_->SetCooperativeLevel(
		WindowsApp::GetInstance()->GetHWND(),
		DISCL_NONEXCLUSIVE | DISCL_FOREGROUND
	);
	assert(SUCCEEDED(result));
}

void MouseInput::Update()
{
	HRESULT result{};
	result = mouse_->Acquire();
	if(FAILED(result)) return;
	result = mouse_->Poll();
	assert(SUCCEEDED(result));

	preKey_ = key_;

	result = mouse_->GetDeviceState(sizeof(key_), &key_);
	assert(SUCCEEDED(result));
}


bool MouseInput::Push(int key)
{
	if(key_.rgbButtons[key] & (MOUSE_ON_VALUE)){
		return true;
	}
	return false;
}

bool MouseInput::Trigger(int key)
{
	if((key_.rgbButtons[key]&(MOUSE_ON_VALUE)) &&
		!(preKey_.rgbButtons[key]&(MOUSE_ON_VALUE))){
		return true;
	}
	return false;
}

bool MouseInput::Release(int key)
{
	if(!(key_.rgbButtons[key]&(MOUSE_ON_VALUE)) &&
		(preKey_.rgbButtons[key]&(MOUSE_ON_VALUE))){
		return true;
	}
	return false;
}


const Vector2 MouseInput::ScreenPosition()
{
	POINT p;
	GetCursorPos(&p);
	ScreenToClient(WindowsApp::GetInstance()->GetHWND(), &p);
	return Vector2{(float)p.x,(float)p.y};
}

const Vector2 MouseInput::MouseVelocity()
{
	return Vector2((float)key_.lX, (float)key_.lY);
}


const float MouseInput::WheelValue()
{
	return (float)key_.lZ;
}
