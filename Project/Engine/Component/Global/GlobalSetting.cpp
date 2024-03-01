#include "GlobalSetting.h"
#include <imgui.h>

#include "WindowsApp.h"
#include "SpriteLoader.h"

#include "KeyboardInput.h"
#include "MouseInput.h"
#include "GamePadInput.h"

GlobalSetting* GlobalSetting::instance_ = nullptr;

GlobalSetting *GlobalSetting::GetInstance()
{
	if(!instance_){
		instance_ = new GlobalSetting;
	}
	return instance_;
}

void GlobalSetting::Update()
{
	ImGui::SetNextWindowPos({WindowsApp::kWindowWidth_ - 300.f,0});
	ImGui::SetNextWindowSize({300,400});
	ImGui::Begin("Global Setting", nullptr, ImGuiWindowFlags_MenuBar);
	ImGui::BeginMenuBar();

	//テクスチャ
	MenuBotton("LoadTex", isTextureSetting_);
	//FPS
	MenuBotton("FPS", isFpsSetting);
	//入力
	MenuBotton("Input", isInputSetting);

	ImGui::EndMenuBar();

	//確認一覧
	LoadTextureDraw();
	FpsDraw();
	InputDraw();

	ImGui::End();
}

void GlobalSetting::MenuBotton(std::string name, int flag)
{
	if(!ImGui::MenuItem(name.c_str())) return;
	isSettingFlag_ = isEnpty_;

	isSettingFlag_ |= flag;
}

void GlobalSetting::LoadTextureDraw()
{
	if(!(isSettingFlag_ & isTextureSetting_)) return;

	for(auto tex : SpriteLoader::GetTexture()){
		if(tex.filePath == "") break;
		ImGui::Text("%d: %s", tex.index, tex.filePath.c_str());
	}
}

void GlobalSetting::FpsDraw()
{
	if(!(isSettingFlag_ & isFpsSetting)) return;

	ImGui::Text("FPS : %lf", DirectXCommon::fps_);
}

void GlobalSetting::InputDraw()
{
	if(!(isSettingFlag_ & isInputSetting)) return;

	if(ImGui::TreeNode("KeyBoard")){
	
		bool isPush = KeyboardInput::GetInstance()->Push(DIK_RETURN);
		bool isTrigger = KeyboardInput::GetInstance()->Trigger(DIK_RETURN);
		bool isRelease = KeyboardInput::GetInstance()->Release(DIK_RETURN);
		ImGui::Text("ENTER");
		ImGui::Checkbox("Key - Push", &isPush);
		ImGui::Checkbox("Key - Trigger", &isTrigger);
		ImGui::Checkbox("Key - Release", &isRelease);

		ImGui::TreePop();
	}
	if(ImGui::TreeNode("Mouse")){
	
		bool isPush = MouseInput::GetInstance()->Push(MouseInput::DIK_MOUSE_LEFT);
		bool isTrigger = MouseInput::GetInstance()->Trigger(MouseInput::DIK_MOUSE_LEFT);
		bool isRelease = MouseInput::GetInstance()->Release(MouseInput::DIK_MOUSE_LEFT);
		ImGui::Text("Left Click");
		ImGui::Checkbox("Push", &isPush);
		ImGui::Checkbox("Trigger", &isTrigger);
		ImGui::Checkbox("Release", &isRelease);


		Vector2 pos = MouseInput::GetInstance()->ScreenPosition();
		Vector2 velocity = MouseInput::GetInstance()->MouseVelocity();
		ImGui::Text("Position X:%f, Y:%f", pos.x,pos.y);
		ImGui::Text("velocity X:%f, Y:%f", velocity.x,velocity.y);

		float wheel = MouseInput::GetInstance()->WheelValue();
		ImGui::Text("Wheel %f", wheel);

		ImGui::TreePop();
	}
	if(ImGui::TreeNode("Pad")){
	
		if(!GamePadInput::GetInstance()->GetIsConnect()){
			ImGui::Text("GamePad NoConnect\n");
		}

		bool isPush = GamePadInput::GetInstance()->ButtonPush(XINPUT_GAMEPAD_A);
		bool isTrigger = GamePadInput::GetInstance()->ButtonTrigger(XINPUT_GAMEPAD_A);
		bool isRelease = GamePadInput::GetInstance()->ButtonRelease(XINPUT_GAMEPAD_A);
		ImGui::Text("A Button");
		ImGui::Checkbox("Push", &isPush);
		ImGui::Checkbox("Trigger", &isTrigger);
		ImGui::Checkbox("Release", &isRelease);

		int trigger = GamePadInput::GetInstance()->LeftTrigger();
		ImGui::Text("LeftTrigger %d", trigger);
		bool isTriggerVaue = GamePadInput::GetInstance()->LeftTrigger(20);
		ImGui::Checkbox("Check", &isTriggerVaue);

		Vector2 lpos = GamePadInput::GetInstance()->LeftStick();
		ImGui::Text("LeftStick X:%f, Y:%f",lpos.x,lpos.y);
		Vector2 rpos = GamePadInput::GetInstance()->RightStick();
		ImGui::Text("RightStick X:%f, Y:%f",rpos.x,rpos.y);

		bool isvibration = false;
		ImGui::Checkbox("Vibration", &isvibration);
		if(isvibration){
			GamePadInput::GetInstance()->VibrationLerp(1);
		}

		ImGui::TreePop();
	}
}