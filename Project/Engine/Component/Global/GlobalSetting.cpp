#include "GlobalSetting.h"
#include <imgui.h>

#include "WindowsApp.h"
#include "SpriteLoader.h"

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

	ImGui::EndMenuBar();

	//確認一覧
	LoadTextureDraw();
	FpsDraw();

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
