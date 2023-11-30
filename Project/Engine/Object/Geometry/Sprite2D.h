#pragma once
#include "Common/SpriteBase.h"
class Sprite2D : public SpriteBase
{
public:
	static Sprite2D* Create(Transform transform = {{0,0,0}, {0,0,0}, {1,1,1}}, std::string filePath = "white1x1.png");

public:
	Sprite2D() = default;
	Sprite2D(Transform transform, std::string filePath){
		this->transform.translate = transform.translate;
		this->transform.rotation = transform.rotation;
		this->transform.scale = transform.scale;

		vertNum_ = 4;
		indexNum_ = 6;

		texturePath_ = filePath;
	}
	void Update();

private:
	void Sprite2DVertexData();
	void Sprite2DIndexData();
};

