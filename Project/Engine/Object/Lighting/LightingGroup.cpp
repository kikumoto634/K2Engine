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

	resource_ = CreateBufferResource(dxCommon->GetDevice(),sizeof(DirectionalLightData));

	resource_->Map(0,nullptr, reinterpret_cast<void**>(&data_));

	//平行光源
	data_->direction = lightDirection_;
	data_->color = lightColor_;
	data_->intensity = lightIntensity;
}

void LightingGroup::Update()
{
	ImGui::Text("Light");
	ImGui::ColorEdit4("Color", &lightColor_.x);
	ImGui::DragFloat3("Dir", &lightDirection_.x, 0.01f);
	ImGui::DragFloat("intencity", &lightIntensity, 0.01f);

	//平行光源
	data_->direction = lightDirection_;
	data_->color = lightColor_;
	data_->intensity = lightIntensity;
}
