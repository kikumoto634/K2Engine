#include "ParticleObject.h"
#include <imgui.h>
#include "LightingGroup.h"
#include "BufferResource.h"

ParticleObject *ParticleObject::Create()
{
	ParticleObject* instance = new ParticleObject();
	instance->Initialize();
	instance->VertexData();
	instance->IndexData();
	return instance;
}

void ParticleObject::Initialize(bool isIndexEnable)
{
	transforms.resize(kNumInstance);
	ParticleBase::Initialize(isIndexEnable);

	for(int i = 0; i < kNumInstance; i++){
		transforms[i].scale = baseTransform.scale;
		transforms[i].rotation = baseTransform.rotation;
		transforms[i].translate = baseTransform.translate + Vector3{i*0.1f, i*0.1f, i*0.1f};
	}
}

void ParticleObject::Update()
{
#ifdef _DEBUG
	ImGui::DragFloat3("Particle - basePos", &baseTransform.translate.x, 0.1f);
	ImGui::DragFloat3("Particle - baseRot", &baseTransform.rotation.x, 0.1f);
	ImGui::DragFloat3("Particle - baseScale", &baseTransform.scale.x, 0.1f);

	if(ImGui::Button("Particle - Update")){
		for(int i = 0; i < kNumInstance; i++){
			transforms[i].scale = baseTransform.scale;
			transforms[i].rotation = baseTransform.rotation;
			transforms[i].translate = baseTransform.translate + Vector3{i*0.1f, i*0.1f, i*0.1f};
		}
	}
#endif // _DEBUG
}

void ParticleObject::VertexData()
{
	vertData_[0].position = {-1.0f, 1.0f, 0.0f, 1.0f};		//左上
	vertData_[0].texcoord = {0.0f, 0.0f};
	vertData_[0].normal = {0.0f, 0.0f, -1.0f};
	
	vertData_[1].position = {1.0f, -1.0f, 0.0f, 1.0f};	//右下
	vertData_[1].texcoord = {1.0f, 1.0f};
	vertData_[1].normal = {0.0f, 0.0f, 1.0f};
	
	vertData_[2].position = {-1.0f, -1.0f, 0.0f, 1.0f};	//左下
	vertData_[2].texcoord = {0.0f, 1.0f};
	vertData_[2].normal = {0.0f, 0.0f, -1.0f};
	
	vertData_[3].position = {1.0f, 1.0f, 0.0f, 1.0f};		//右上
	vertData_[3].texcoord = {1.0f, 0.0f};
	vertData_[3].normal = {0.0f, 0.0f, -1.0f};
}

void ParticleObject::IndexData()
{
	indexData_[0] = 0;	indexData_[1] = 1;	indexData_[2] = 2;
	indexData_[3] = 0;	indexData_[4] = 3;	indexData_[5] = 1;
}
