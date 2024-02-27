#pragma once
#include "BaseScene.h"

#include "FollowCamera.h"
#include "LightingGroup.h"

#include "Geometry/ObjModel.h"

class Sample2Scene : public BaseScene
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

