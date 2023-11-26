#pragma once
#include "WindowsApp.h"
#include "DirectXCommon.h"
#include "ShadowCommon.h"

#include "Application.h"

#include "../Tool/ImGui/ImGuiManager.h"

class FrameWork
{
public:
	static FrameWork* Create();

public:
	~FrameWork(){
		delete imgui_;
		delete app_;
		delete shadowCommon_;
		DirectXCommon::Finalize();
		WindowsApp::Finalize();
	}

	void Run();

private:
	void Initialize();

private:
	WindowsApp* win_ = nullptr;
	DirectXCommon* dxCommon_ = nullptr;
	ShadowCommon* shadowCommon_ = nullptr;

	Application* app_ = nullptr;

	ImGuiManager* imgui_ = nullptr;
};

