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
	particles_.resize(kNumMaxInstance_);
	ParticleBase::Initialize(isIndexEnable);

	//乱数生成器
	std::random_device seedGenerator_;
	std::mt19937 randomEngine(seedGenerator_());

	std::uniform_real_distribution<float> distribution(-1.0f,1.0f);
	for(int i = 0; i < kNumMaxInstance_; i++){
		particles_[i] = MakeNewParticle(randomEngine);
	}
}

void ParticleObject::Update()
{
#ifdef _DEBUG
	/*ImGui::DragFloat3("Particle - basePos", &baseParticle_.transform.translate.x, 0.1f);
	ImGui::DragFloat3("Particle - baseRot", &baseParticle_.transform.rotation.x, 0.1f);
	ImGui::DragFloat3("Particle - baseScale", &baseParticle_.transform.scale.x, 0.1f);*/
#endif // _DEBUG

	for(int i = 0; i < kNumMaxInstance_; i++){
		particles_[i].transform.translate += particles_[i].velocity * kDeltaTime_;
		particles_[i].currentTime += kDeltaTime_;
	}
}

void ParticleObject::VertexData()
{
	vertData_[0].position = {-1.0f, 1.0f, 0.0f, 1.0f};		//左上
	vertData_[0].texcoord = {0.0f, 0.0f};
	
	vertData_[1].position = {1.0f, -1.0f, 0.0f, 1.0f};	//右下
	vertData_[1].texcoord = {1.0f, 1.0f};
	
	vertData_[2].position = {-1.0f, -1.0f, 0.0f, 1.0f};	//左下
	vertData_[2].texcoord = {0.0f, 1.0f};
	
	vertData_[3].position = {1.0f, 1.0f, 0.0f, 1.0f};		//右上
	vertData_[3].texcoord = {1.0f, 0.0f};
}

void ParticleObject::IndexData()
{
	indexData_[0] = 0;	indexData_[1] = 1;	indexData_[2] = 2;
	indexData_[3] = 0;	indexData_[4] = 3;	indexData_[5] = 1;
}

ParticleData ParticleObject::MakeNewParticle(std::mt19937 &randomEngine)
{
	std::uniform_real_distribution<float> distribution(-1.0f,1.0f);
	std::uniform_real_distribution<float> distColor(0.0f,1.0f);
	std::uniform_real_distribution<float> distTime(1.0f,3.0f);
	ParticleData particle;

	particle.transform.scale = {1,1,1};
	particle.transform.rotation = {0,0,0};
	particle.transform.translate = {distribution(randomEngine),distribution(randomEngine),distribution(randomEngine)};
	particle.velocity = {distribution(randomEngine),distribution(randomEngine),distribution(randomEngine)};

	particle.color = {distColor(randomEngine),distColor(randomEngine),distColor(randomEngine), 1.0f};
	
	particle.lifeTime = distTime(randomEngine);
	particle.currentTime = 0;

	return particle;
}
