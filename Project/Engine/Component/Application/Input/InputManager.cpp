#include "InputManager.h"
#include <cassert>

#include "WindowsApp.h"

#include "KeyboardInput.h"
#include "MouseInput.h"
#include "GamePadInput.h"

#pragma comment(lib, "dinput8.lib")

InputManager* InputManager::instance_ = nullptr;

InputManager::~InputManager()
{
	gamePad_->VibrationStop();
	GamePadInput::Finalize();
	MouseInput::Finalize();
	KeyboardInput::Finalize();
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
	mouse_ = MouseInput::GetInstance();
	gamePad_ = GamePadInput::GetInstance();

	///DirectInPut初期化
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
	mouse_->Initialize(directInput_);
	gamePad_->Initialize();
}

void InputManager::Update()
{
	keyboard_->Update();
	mouse_->Update();
	gamePad_->Update();
}

