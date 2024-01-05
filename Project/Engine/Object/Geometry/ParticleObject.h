#pragma once
#include "Common/ParticleBase.h"
#include <random>

class ParticleObject : public ParticleBase
{
public:
	struct Emitter{
		Transform transform; // エミッタのTransform
		uint32_t count;	//発生数
		float frequency; //発生頻度
		float frequencyTime; //頻度用時刻
	};

public:
	static ParticleObject* Create();

public:
	ParticleObject(){
		vertNum_ = 4;
		indexNum_ = 6;

		texturePath_ = "white1x1.png";
	}
	void Update();

	void Add(const Vector3& translate = {0,0,0});

private:
	void Initialize(bool isIndexEnable = true) override;

	void VertexData();
	void IndexData();

	ParticleData MakeNewParticle(std::mt19937& randomEngine, const Vector3& translate);
	std::list<ParticleData> Emission(const Emitter& emitter, std::mt19937& randomEngine, const Vector3& translate);

private:
	const float kDeltaTime_ = 1.0f/60.f;

	Emitter emitter_ = {{},3,0,0};
	bool isEmitterBegin_ = false;
};

