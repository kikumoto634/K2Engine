#pragma once
#include "BaseScene.h"

class Sample1Scene : public BaseScene
{
public:
	void Initialize() override;
	void Update() override;
	void Finalize() override;

	void GeometryDraw() override;
	void SpriteDraw() override;
	void ParticleDraw() override;

private:
	FollowCamera* camera_ = nullptr;

	ObjModel* monkey_ = nullptr;
};

