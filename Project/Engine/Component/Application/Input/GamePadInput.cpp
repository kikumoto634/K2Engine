#include "GamePadInput.h"

#include "Easing.h"

#pragma comment(lib, "xinput.lib")

GamePadInput* GamePadInput::instance_ = nullptr;

GamePadInput *GamePadInput::GetInstance()
{
	if(!instance_){
		instance_ = new GamePadInput();
	}
	return instance_;
}

void GamePadInput::Finalize()
{
	if(instance_){
		delete instance_;
		instance_ = nullptr;
	}
}


void GamePadInput::Initialize()
{
	ZeroMemory(&state_, sizeof(XINPUT_STATE));
	ZeroMemory(&vibration_, sizeof(XINPUT_VIBRATION));
}

void GamePadInput::Update()
{
	preState_ = state_;
	//接続
	DWORD result = XInputGetState(0,&state_);
	if(result == ERROR_SUCCESS){
		isConnect_ = true;
	}
	else{
		isConnect_ = false;
		return;
	}

	StickUpdate();
	VibrationUpdate();
}


bool GamePadInput::ButtonPush(int key)
{
	if(!isConnect_) return false;

	if(state_.Gamepad.wButtons & key){
		return true;
	}

	return false;
}

bool GamePadInput::ButtonTrigger(int key)
{
	if(!isConnect_) return false;

	if((state_.Gamepad.wButtons & key) &&
		!(preState_.Gamepad.wButtons & key)){
		return true;
	}

	return false;
}

bool GamePadInput::ButtonRelease(int key)
{
	if(!isConnect_) return false;

	if(!(state_.Gamepad.wButtons & key) &&
		(preState_.Gamepad.wButtons & key)){
		return true;
	}

	return false;
}


const int GamePadInput::LeftTrigger()
{
	if(!isConnect_) return 0;

	return (int)state_.Gamepad.bLeftTrigger;
}
const bool GamePadInput::LeftTrigger(const int value)
{
	if(!isConnect_) return false;

	if(state_.Gamepad.bLeftTrigger > value){
		return true;
	}
	return false;
}

const int GamePadInput::RightTrigger()
{
	if(!isConnect_) return 0;

	return (int)state_.Gamepad.bRightTrigger;
}
const bool GamePadInput::RightTrigger(const int value)
{
	if(!isConnect_) return false;

	if(state_.Gamepad.bRightTrigger > value){
		return true;
	}
	return false;
}


void GamePadInput::StickUpdate()
{
	//デッドゾーン設定
	if(!isConnect_) return;

	//デッドゾーン
	if(state_.Gamepad.sThumbLX<XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
		state_.Gamepad.sThumbLX>-XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
	{
		state_.Gamepad.sThumbLX = 0;
	}
	if(state_.Gamepad.sThumbLY<XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE && 
		state_.Gamepad.sThumbLY>-XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
	{
		state_.Gamepad.sThumbLY = 0;
	}

	if(state_.Gamepad.sThumbRX<XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE 
		&& state_.Gamepad.sThumbRX>-XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
	{
		state_.Gamepad.sThumbRX = 0;
	}
	if(state_.Gamepad.sThumbRY<XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
		state_.Gamepad.sThumbRY>-XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
	{
		state_.Gamepad.sThumbRY = 0;
	}
}

const Vector2 GamePadInput::LeftStick()
{
	if(!isConnect_)	return Vector2();

	float x = state_.Gamepad.sThumbLX;
	float y = state_.Gamepad.sThumbLY;

	//Horizontal
	if(state_.Gamepad.sThumbLX > 0.1){
		x -= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE - 1;
		x /= StickMaxValue_;
	}
	else if(state_.Gamepad.sThumbLX < -0.1){
		x += XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
		x /= StickMaxValue_;
	}

	//Vertical
	if(state_.Gamepad.sThumbLY > 0.1){
		y -= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE - 1;
		y /= StickMaxValue_;
	}
	else if(state_.Gamepad.sThumbLY < -0.1){
		y += XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
		y /= StickMaxValue_;
	}

	return Vector2(x,y);
}

const Vector2 GamePadInput::RightStick()
{
	if(!isConnect_)	return Vector2();

	float x = state_.Gamepad.sThumbRX;
	float y = state_.Gamepad.sThumbRY;

	//Horizontal
	if(state_.Gamepad.sThumbRX > 0.1){
		x -= XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE - 1;
		x /= StickMaxValue_;
	}
	else if(state_.Gamepad.sThumbRX < -0.1){
		x += XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
		x /= StickMaxValue_;
	}

	//Vertical
	if(state_.Gamepad.sThumbRY > 0.1){
		y -= XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE - 1;
		y /= StickMaxValue_;
	}
	else if(state_.Gamepad.sThumbRY < -0.1){
		y += XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
		y /= StickMaxValue_;
	}

	return Vector2(x,y);
}


void GamePadInput::VibrationUpdate()
{
	if(!isConnect_) return;
	if(!isVibrationLeap_) return;

	vibrationValue_ = Easing_Point2_Linear<Vector2>({
		(float)VibrationMaxValue_,(float)VibrationMaxValue_},
		{0.0f,0.0f},
		Time_OneWay(vibrationTime_, vibrationTimeMax_)
	);

	vibration_.wLeftMotorSpeed = (int)vibrationValue_.x;
	vibration_.wRightMotorSpeed = (int)vibrationValue_.y;
	XInputSetState(0, &vibration_);

	if(vibrationTime_ < 1.0f) return;
	isVibrationLeap_ = false;
	vibrationTime_ = 0.0f;
	VibrationStop();
}

void GamePadInput::VibrationStart()
{
	if(!isConnect_) return;
	vibration_.wLeftMotorSpeed = VibrationMaxValue_;
	vibration_.wRightMotorSpeed = VibrationMaxValue_;
	XInputSetState(0, &vibration_);
}

void GamePadInput::VibrationStop()
{
	if(!isConnect_) return;
	vibration_.wLeftMotorSpeed = 0;
	vibration_.wRightMotorSpeed = 0;
	XInputSetState(0,&vibration_);
}

void GamePadInput::VibrationLerp(const float second)
{
	if(!isConnect_) return;
	vibrationTimeMax_ = second;
	isVibrationLeap_ = true;
}
