#pragma once
#include "Geometry/Common/SpriteBase.h"

class PostEffect : public SpriteBase
{
public:
	static PostEffect* Create(Transform transform = {{0,0,0}, {0,0,0}, {1,1,1}});

public:
	PostEffect() = default;
	PostEffect(Transform transform){
		this->transform.translate = transform.translate;
		this->transform.rotation = transform.rotation;
		this->transform.scale = transform.scale;

		vertNum_ = 4;
		indexNum_ = 6;
	}
	void Update();

private:
	void PostEffectVertexData();
	void PostEffectIndexData();

private:
	Vector2 size = {100.f,100.f};
};

