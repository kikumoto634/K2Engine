#pragma once
#include "Common/ParticleBase.h"
#include <random>

class ParticleObject : public ParticleBase
{
public:
	static ParticleObject* Create();

public:
	ParticleObject(){
		vertNum_ = 4;
		indexNum_ = 6;

		kNumMaxInstance_ = 10;
	}
	void Update();

private:
	void Initialize(bool isIndexEnable = true) override;

	void VertexData();
	void IndexData();

	ParticleData MakeNewParticle(std::mt19937& randomEngine);

private:
	const float kDeltaTime_ = 1.0f/60.f;
};

