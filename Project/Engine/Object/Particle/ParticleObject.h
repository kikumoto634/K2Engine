#pragma once
#include "Common/ParticleBase.h"

class ParticleObject : public ParticleBase
{
public:
	static ParticleObject* Create();

public:
	ParticleObject(){
		vertNum_ = 4;
		indexNum_ = 6;

		//texturePath_ = "white1x1.png";
	}
	//void Initialize(bool isIndexEnable = true);
	void Update();

	void Add(const Vector3& translate = {0,0,0}) override;

private:
	void Initialize(bool isIndexEnable = true) override;

	void VertexData();
	void IndexData();

	ParticleData MakeNewParticle(std::mt19937& randomEngine, const Vector3& translate) override;
};

