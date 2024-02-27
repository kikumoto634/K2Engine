#pragma once
class BaseScene
{
public:
	virtual void Initialize() {};

	virtual void Update() {};

	virtual void Finalize() {};

	virtual void GeometryDraw() {};
	virtual void SpriteDraw() {};
	virtual void ParticleDraw() {};
};

