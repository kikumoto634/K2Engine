#pragma once
#include "Common/EmitterFrameBase.h"

#include "ParticleDatas/EmitterData.h"

class EmitterFrameSquare : public EmitterFrameBase
{
public:
	static EmitterFrameSquare* Create();

public:
	EmitterFrameSquare(){
		vertNum_ = 8;
		indexNum_ = 36;
	}

	void Update(EmitterData emitter);

private:
	void VertexData();
	void IndexData();
};

