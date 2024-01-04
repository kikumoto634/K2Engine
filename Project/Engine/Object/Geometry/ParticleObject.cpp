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
	particles_.resize(kNumInstance_);
	ParticleBase::Initialize(isIndexEnable);

	for(int i = 0; i < kNumInstance_; i++){
		particles_[i].transform.scale = baseParticle_.transform.scale;
		particles_[i].transform.rotation = baseParticle_.transform.rotation;
		particles_[i].transform.translate = baseParticle_.transform.translate + Vector3{i*0.1f, i*0.1f, i*0.1f};

		particles_[i].velocity = {0,1,0};
	}
}

void ParticleObject::Update()
{
#ifdef _DEBUG
	ImGui::DragFloat3("Particle - basePos", &baseParticle_.transform.translate.x, 0.1f);
	ImGui::DragFloat3("Particle - baseRot", &baseParticle_.transform.rotation.x, 0.1f);
	ImGui::DragFloat3("Particle - baseScale", &baseParticle_.transform.scale.x, 0.1f);
#endif // _DEBUG

	for(int i = 0; i < kNumInstance_; i++){
		particles_[i].transform.translate += particles_[i].velocity * kDeltaTime_;
	}
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
