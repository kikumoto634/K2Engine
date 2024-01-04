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

		kNumInstance = 10;
	}
	void Update();

private:
	void Initialize(bool isIndexEnable = true) override;

	void VertexData();
	void IndexData();
};

