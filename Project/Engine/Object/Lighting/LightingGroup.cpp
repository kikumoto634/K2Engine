#include "LightingGroup.h"
#include "BufferResource.h"

#include <imgui.h>

LightingGroup* LightingGroup::instance_ = nullptr;

LightingGroup *LightingGroup::Create()
{
	if(instance_ == nullptr){
		instance_ = new LightingGroup();
		instance_->Initialize();
	}
	return instance_;
}

LightingGroup *LightingGroup::GetInstance()
{
	if(instance_ == nullptr){
		instance_ = new LightingGroup();
	}
	return instance_;
}

void LightingGroup::Initialize()
{
	dxCommon = DirectXCommon::GetInstance();

	view = view.Inverse(GetWorldMatrix());
	proj = MakeOrthographicMatrix(0,0,
		(float)WindowsApp::kWindowWidth_, (float)WindowsApp::kWindowHeight_,
		0.1f, 1.f);

	resource_ = CreateBufferResource(dxCommon->GetDevice(),sizeof(DirectionalLightData));

	resource_->Map(0,nullptr, reinterpret_cast<void**>(&data_));

	//平行光源
	data_->direction = rotation;
	data_->VP = view*proj;
	data_->color = lightColor_;
	data_->intensity = lightIntensity;
}

void LightingGroup::Update()
{
	ImGui::Text("Light");
	ImGui::ColorEdit4("Color", &lightColor_.x);
	ImGui::DragFloat3("Pos", &translate.x, 1.f);
	ImGui::DragFloat3("Dir", &rotation.x, 1.f);
	ImGui::DragFloat("intencity", &lightIntensity, 0.01f);

	//平行光源
	data_->direction = DirectionalVector3FromDegrees((rotation));
	data_->color = lightColor_;
	data_->intensity = lightIntensity;
}
