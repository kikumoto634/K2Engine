#pragma once
#include "Common/ParticleBase.h"
#include "EmitterData.h"

class ParticleEmitterObject : public ParticleBase
{
public:
	static ParticleEmitterObject* Create();

public:
	ParticleEmitterObject(){
		vertNum_ = 4;
		indexNum_ = 6;

		//texturePath_ = "white1x1.png";
	}
	void Update() override;
	void Draw(Camera* camera) override;

	void Add(const Vector3& translate = {0,0,0}) override;

private:
	//void Initialize(bool isIndexEnable = true) override;

	void VertexData();
	void IndexData();

	ParticleData MakeNewParticle(std::mt19937& randomEngine, const Vector3& translate) override;
	std::list<ParticleData> Emission(const Emitter& emitter, std::mt19937& randomEngine, const Vector3& translate);

private:
	std::list<ParticleData> particles_;

	Emitter emitter_ = {{{0,0,0},{0,0,0},{1,1,1}},{-1,1},3,0.5f,0.0f};
};