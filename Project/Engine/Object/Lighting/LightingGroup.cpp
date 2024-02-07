#include "LightingGroup.h"
#include "BufferResource.h"

#include <imgui.h>
#include "GlobalVariables.h"

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
	data_->direction = rotation;
	data_->color = lightColor_;
	data_->intensity = lightIntensity_;

	ApplyGlobalVariablesInitialize();
}

void LightingGroup::Update()
{
	ApplyGlobalVariablesUpdate();

	//平行光源
	data_->direction = rotation;
	data_->color = lightColor_;
	data_->intensity = lightIntensity_;
}

void LightingGroup::ApplyGlobalVariablesInitialize()
{
#ifdef _DEBUG
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* name = "Light";
	//グループを追加
	GlobalVariables::GetInstance()->CreateGroup(name);
	globalVariables->AddItem(name, "1.direction", rotation);
	globalVariables->AddItem(name, "2.color", lightColor_);
	globalVariables->AddItem(name, "3.LightIntensity", lightIntensity_);
	globalVariables->AddItem(name, "4.SpecularIntensity", specularPower);
#endif // _DEBUG
}

void LightingGroup::ApplyGlobalVariablesUpdate()
{
#ifdef _DEBUG
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* name = "Light";
	rotation = globalVariables->GetVector3Value(name, "1.direction");
	lightColor_ = globalVariables->GetVector4Value(name, "2.color");
	lightIntensity_ = globalVariables->GetFloatValue(name, "3.LightIntensity");
	specularPower = globalVariables->GetFloatValue(name, "4.SpecularIntensity");
#endif // _DEBUG
}