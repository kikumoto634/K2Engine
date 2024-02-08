#pragma once
#include "WindowsApp.h"
#include "DirectXCommon.h"

#include "Input.h"

#include "Application.h"

#include "../Tool/ImGui/ImGuiManager.h"

#include "Geometry/Common/SpriteCommon.h"
#include "Geometry/Common/GeometryCommon.h"
#include "Particle/Common/GPUParticleCommon.h"

class FrameWork
{
public:
	static FrameWork* Create();

public:
	~FrameWork(){
		GPUParticleCommon::Finalize();
		GeometryCommon::Finalize();
		SpriteCommon::Finalize();
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

	Input* input_ = nullptr;

	Application* app_ = nullptr;

	ImGuiManager* imgui_ = nullptr;
};

