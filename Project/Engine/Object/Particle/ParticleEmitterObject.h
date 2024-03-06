#pragma once
#include "EmitterFrame/EmitterFrameSquare.h"

#include "Base/ParticleBase.h"

#include "ParticleDatas/EmitterData.h"

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

	void Add();
	void Add(const Vector3& translate);

private:
	void Initialize(bool isIndexEnable = true) override;

	void VertexData();
	void IndexData();

	ParticleData MakeNewParticle(std::mt19937& randomEngine, const Vector3& translate) override;
	std::list<ParticleData> Emission(const EmitterData& emitter, std::mt19937& randomEngine, const Vector3& translate);

private:
	EmitterData emitter_ = {{{0,0,0},{0,0,0},{1,1,1}},{-1,1},3,0.5f,0.0f};
	EmitterFrameSquare* frame_ = nullptr;
};