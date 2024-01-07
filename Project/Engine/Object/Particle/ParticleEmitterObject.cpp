#include "ParticleEmitterObject.h"
#include <imgui.h>
#include "MathUtility.h"

ParticleEmitterObject *ParticleEmitterObject::Create()
{
	ParticleEmitterObject* instance = new ParticleEmitterObject();
	instance->Initialize();
	instance->VertexData();
	instance->IndexData();
	return instance;
}

void ParticleEmitterObject::Initialize(bool isIndexEnable)
{
	ParticleBase::Initialize(isIndexEnable);

	emitter_.velocity = {-1,1};
	emitter_.count = 1;
	emitter_.frequency = 0.5f;
	emitter_.frequencyTime = 0.0f;

	frame_ = EmitterFrameSquare::Create();
}

void ParticleEmitterObject::Update()
{
#ifdef _DEBUG
	ImGui::Text("Emitter");
	ImGui::DragFloat3("Pos   - Emitter", &emitter_.transform.translate.x, 0.01f);
	ImGui::DragFloat3("Rot   - Emitter", &emitter_.transform.rotation.x, 1.f);
	ImGui::DragFloat3("Scale - Emitter", &emitter_.transform.scale.x, 0.01f);
	for(int i = 0; i < 3; i++){
		if(i > 0){ImGui::SameLine();}

		const char* str1 = "E ";
		std::string strA = std::string(str1) + billboardTypeName[i];

		ImGui::Checkbox(strA.c_str(), &billboardTypeEnable[i]);
	}
	ImGui::DragInt("count - Emitter", &emitter_.count, 1, 0,100);
	
#endif // _DEBUG

	frame_->Update(emitter_);

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

void ParticleEmitterObject::Draw(Camera *camera)
{
	frame_->Draw(camera);
	ParticleBase::Draw(camera);
}


void ParticleEmitterObject::Add()
{
	//乱数生成器
	std::random_device seedGenerator_;
	std::mt19937 randomEngine(seedGenerator_());
	emitter_.frequencyTime += kDeltaTime_;
	if(emitter_.frequency <= emitter_.frequencyTime){
		particles_.splice(particles_.end(), Emission(emitter_, randomEngine, emitter_.transform.translate));
		emitter_.frequencyTime -= emitter_.frequency;
	}
}

void ParticleEmitterObject::Add(const Vector3& translate)
{
	emitter_.transform.translate = translate;

	//乱数生成器
	std::random_device seedGenerator_;
	std::mt19937 randomEngine(seedGenerator_());
	emitter_.frequencyTime += kDeltaTime_;
	if(emitter_.frequency <= emitter_.frequencyTime){
		particles_.splice(particles_.end(), Emission(emitter_, randomEngine, emitter_.transform.translate));
		emitter_.frequencyTime -= emitter_.frequency;
	}
}


void ParticleEmitterObject::VertexData()
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

void ParticleEmitterObject::IndexData()
{
	indexData_[0] = 0;	indexData_[1] = 1;	indexData_[2] = 2;
	indexData_[3] = 0;	indexData_[4] = 3;	indexData_[5] = 1;
}


ParticleData ParticleEmitterObject::MakeNewParticle(std::mt19937 &randomEngine, const Vector3& translate)
{
	std::uniform_real_distribution<float> distSizeX(-emitter_.transform.scale.x/2.f ,emitter_.transform.scale.x/2.f);
	std::uniform_real_distribution<float> distSizeY(-emitter_.transform.scale.y/2.f ,emitter_.transform.scale.y/2.f);
	std::uniform_real_distribution<float> distSizeZ(-emitter_.transform.scale.z/2.f ,emitter_.transform.scale.z/2.f);

	std::uniform_real_distribution<float> distVelocity(emitter_.velocity.x,emitter_.velocity.y);
	std::uniform_real_distribution<float> distColor(0.0f,1.0f);
	std::uniform_real_distribution<float> distTime(1.0f,3.0f);
	
	ParticleData particle;

	particle.transform.scale = {1,1,1};
	particle.transform.rotation = {0,0,0};
	particle.transform.translate = translate + Multiplication(Vector3{distSizeX(randomEngine),distSizeY(randomEngine),distSizeZ(randomEngine)}, emitter_.transform.GetRotMatrix());

	particle.velocity = {distVelocity(randomEngine),distVelocity(randomEngine),distVelocity(randomEngine)};

	particle.color = {distColor(randomEngine),distColor(randomEngine),distColor(randomEngine), 1.0f};
	
	particle.lifeTime = distTime(randomEngine);
	particle.currentTime = 0;

	return particle;
}

std::list<ParticleData> ParticleEmitterObject::Emission(const EmitterData &emitter, std::mt19937 &randomEngine, const Vector3& translate)
{
	std::list<ParticleData> particles;
	for(int i = 0; i < emitter.count; i++){
		particles.push_back(MakeNewParticle(randomEngine, translate));
	}

	return particles;
}
