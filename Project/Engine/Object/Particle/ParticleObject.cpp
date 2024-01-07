#include "ParticleObject.h"
#include <imgui.h>

ParticleObject *ParticleObject::Create()
{
	ParticleObject* instance = new ParticleObject();
	instance->Initialize();
	instance->VertexData();
	instance->IndexData();
	return instance;
}

void ParticleObject::Update()
{
#ifdef _DEBUG
	ImGui::Text("Particle");
	for(int i = 0; i < 3; i++){
		if(i > 0){ImGui::SameLine();}

		const char* str1 = "P ";
		std::string strA = std::string(str1) + billboardTypeName[i];

		ImGui::Checkbox(strA.c_str(), &billboardTypeEnable[i]);
	}
#endif // _DEBUG

	ParticleBase::Update();
}

void ParticleObject::Add(const Vector3& translate)
{
	//乱数生成器
	std::random_device seedGenerator_;
	std::mt19937 randomEngine(seedGenerator_());
	particles_.push_back(MakeNewParticle(randomEngine, translate));
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
	std::uniform_real_distribution<float> distVelocity(0.0f,1.0f);
	std::uniform_real_distribution<float> distColor(0.0f,1.0f);
	std::uniform_real_distribution<float> distTime(1.0f,3.0f);
	ParticleData particle;

	particle.transform.scale = {1,1,1};
	particle.transform.rotation = {0,0,0};
	particle.transform.translate = translate;

	particle.velocity = {distVelocity(randomEngine),distVelocity(randomEngine),distVelocity(randomEngine)};

	particle.color = {distColor(randomEngine),distColor(randomEngine),distColor(randomEngine), 1.0f};
	
	particle.lifeTime = distTime(randomEngine);
	particle.currentTime = 0;

	return particle;
}
