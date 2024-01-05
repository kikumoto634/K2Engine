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
	ParticleBase::Initialize(isIndexEnable);

	emitter_.count = 3;
	emitter_.frequency = 0.5f;
	emitter_.frequencyTime = 0.0f;
}

void ParticleObject::Update()
{
#ifdef _DEBUG
	ImGui::Checkbox("Add Particle",&isEmitterBegin_);
	if(isEmitterBegin_){
		Add();
	}
	for(int i = 0; i < 3; i++){
		if(i > 0){ImGui::SameLine();}
		ImGui::Checkbox(billboardTypeName[i], &billboardTypeEnable[i]);
	}
#endif // _DEBUG

	for(std::list<ParticleData>::iterator particleIterator = particles_.begin(); particleIterator != particles_.end();){
		if(particleIterator->lifeTime <= particleIterator->currentTime) {
			particleIterator = particles_.erase(particleIterator);
			continue;
		}

		particleIterator->transform.translate += particleIterator->velocity * kDeltaTime_;
		particleIterator->transform.rotation.z += particleIterator->velocity.z * kDeltaTime_;
		particleIterator->currentTime += kDeltaTime_;

		++particleIterator;
	}
}

void ParticleObject::Add(const Vector3& translate)
{
	//乱数生成器
	std::random_device seedGenerator_;
	std::mt19937 randomEngine(seedGenerator_());

	emitter_.frequencyTime += kDeltaTime_;
	if(emitter_.frequency <= emitter_.frequencyTime){
		particles_.splice(particles_.end(), Emission(emitter_, randomEngine, translate));
		emitter_.frequencyTime -= emitter_.frequency;
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

ParticleData ParticleObject::MakeNewParticle(std::mt19937 &randomEngine, const Vector3& translate)
{
	std::uniform_real_distribution<float> distribution(-1.0f,1.0f);
	std::uniform_real_distribution<float> distColor(0.0f,1.0f);
	std::uniform_real_distribution<float> distTime(1.0f,3.0f);
	ParticleData particle;

	Vector3 randomTranslate = {distribution(randomEngine),distribution(randomEngine),distribution(randomEngine)};

	particle.transform.scale = {1,1,1};
	particle.transform.rotation = {0,0,0};
	particle.transform.translate = randomTranslate + translate;

	particle.velocity = {distribution(randomEngine),distribution(randomEngine),distribution(randomEngine)};

	particle.color = {distColor(randomEngine),distColor(randomEngine),distColor(randomEngine), 1.0f};
	
	particle.lifeTime = distTime(randomEngine);
	particle.currentTime = 0;

	return particle;
}

std::list<ParticleData> ParticleObject::Emission(const Emitter &emitter, std::mt19937 &randomEngine, const Vector3& translate)
{
	std::list<ParticleData> particles;
	for(uint32_t i = 0; i < emitter.count; i++){
		particles.push_back(MakeNewParticle(randomEngine, translate));
		particles.push_back(MakeNewParticle(randomEngine, translate));
	}

	return particles;
}
