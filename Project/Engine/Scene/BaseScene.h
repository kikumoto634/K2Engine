#pragma once
class BaseScene
{
public:
	virtual void Initialize() = 0;

	virtual void Update() = 0;

	virtual void GeometryDraw() = 0;
	virtual void SpriteDraw() {};
	virtual void ParticleDraw() {};
};

