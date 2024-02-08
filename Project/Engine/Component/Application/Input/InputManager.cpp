#include "InputManager.h"
#include "Easing.h"
#include <cassert>

#include "WindowsApp.h"

#include "KeyboardInput.h"
#include "GamePadInput.h"

#pragma comment(lib, "dinput8.lib")

InputManager* InputManager::instance_ = nullptr;


InputManager::~InputManager()
{
	gamePad_->VibrationStop();
	delete instance_;
}

InputManager *InputManager::GetInstance()
{
	if(!instance_){
		instance_ = new InputManager();
		instance_->Initialize();
	}
	return instance_;
}

void InputManager::Initialize()
{
	HRESULT result;

	window = WindowsApp::GetInstance();

	keyboard_ = KeyboardInput::GetInstance();
	gamePad_ = GamePadInput::GetInstance();

	///DirectInPut
	//初期化 (他入力方法追加でもこのオブジェクトは一つのみ)
	HINSTANCE hInstance = GetModuleHandle(nullptr);
	result = DirectInput8Create(
		hInstance, 
		DIRECTINPUT_VERSION, 
		IID_IDirectInput8,
		(void**)&directInput_, 
		nullptr
	);
	assert(SUCCEEDED(result));


	keyboard_->Initialize(directInput_);
	gamePad_->Initialize();

	//マウス
	result = directInput_->CreateDevice(
		GUID_SysMouse,
		&mouse_,
		NULL
	);
	assert(SUCCEEDED(result));



	//マウス
	result = mouse_->SetDataFormat(&c_dfDIMouse);
	assert(SUCCEEDED(result));
	
	//マウス
	result = mouse_->SetCooperativeLevel(
		window->GetHWND(),
		DISCL_NONEXCLUSIVE | DISCL_FOREGROUND
	);
	assert(SUCCEEDED(result));
}

void InputManager::Update()
{
	HRESULT result;

	keyboard_->Update();
	gamePad_->Update();

	//動作開始
	result = mouse_->Acquire();
	result = mouse_->Poll();
	//assert(SUCCEEDED(result));

	//前回のキー入力情報コピー
	mousePreKey_ = mouseKey_;


	//状態取得
	//assert(SUCCEEDED(result));
	result = mouse_->GetDeviceState(sizeof(mouseKey_), &mouseKey_);
}

bool InputManager::MousePush(int keyNumber)
{
	if(mouseKey_.rgbButtons[keyNumber] & (MOUSE_ON_VALUE)){
		return true;
	}
	return false;
}

bool InputManager::MouseTrigger(int keyNumber)
{
	if((mouseKey_.rgbButtons[keyNumber] & (MOUSE_ON_VALUE)) && !(mousePreKey_.rgbButtons[keyNumber] & (MOUSE_ON_VALUE))){
		return true;
	}
	return false;
}

const Vector2 InputManager::GetMousePos()
{
	POINT p;
	GetCursorPos(&p);
	ScreenToClient(window->GetHWND(), &p);
	return Vector2{(float)p.x,(float)p.y};
}

const Vector2 InputManager::GetMouseVelocity()
{
	return Vector2((float)mouseKey_.lX, (float)mouseKey_.lY);
}

const float InputManager::GetMouseWheel()
{
	return (float)mouseKey_.lZ;
}


