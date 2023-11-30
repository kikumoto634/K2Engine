#pragma once
#include "WindowsApp.h"
#include "DirectXCommon.h"

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
		DirectXCommon::Finalize();
		WindowsApp::Finalize();
	}

	void Run();

private:
	void Initialize();

private:
	WindowsApp* win_ = nullptr;
	DirectXCommon* dxCommon_ = nullptr;

	Application* app_ = nullptr;

	ImGuiManager* imgui_ = nullptr;
};

