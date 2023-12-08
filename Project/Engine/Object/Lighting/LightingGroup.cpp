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

	ApplyGlobalVariablesInitialize();
}

void LightingGroup::Update()
{
	ApplyGlobalVariablesUpdate();

	//平行光源
	data_->direction = DirectionalVector3FromDegrees(rotation);
	data_->color = lightColor_;
	data_->intensity = lightIntensity;
}

void LightingGroup::ApplyGlobalVariablesInitialize()
{
#ifdef _DEBUG
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* name = "Light";
	//グループを追加
	GlobalVariables::GetInstance()->CreateGroup(name);
	globalVariables->AddItem(name, "0.translate", translate);
	globalVariables->AddItem(name, "1.rotate", rotation);
	globalVariables->AddItem(name, "2.scale", scale);
	globalVariables->AddItem(name, "3.color", lightColor_);
#endif // _DEBUG
}

void LightingGroup::ApplyGlobalVariablesUpdate()
{
#ifdef _DEBUG
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* name = "Light";
	translate = globalVariables->GetVector3Value(name, "0.translate");
	rotation = globalVariables->GetVector3Value(name, "1.rotate");
	scale = globalVariables->GetVector3Value(name, "2.scale");
	lightColor_ = globalVariables->GetVector4Value(name, "3.color");
#endif // _DEBUG
}