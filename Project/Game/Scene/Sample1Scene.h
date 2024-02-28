#pragma once
#include "BaseScene.h"

#include <vector>

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

	const int Width = 10;
	const int Height = 10;
	std::vector<Sprite3D*> mapSp_;
};

