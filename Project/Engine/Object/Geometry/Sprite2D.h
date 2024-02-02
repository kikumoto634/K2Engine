#pragma once
#include "Base/SpriteBase.h"
class Sprite2D : public SpriteBase
{
public:
	static Sprite2D* Create(Transform transform = {{100,100,0}, {0,0,0}, {1,1,1}});

public:
	Sprite2D(Transform transform){
		translate = transform.translate;
		rotation = transform.rotation;
		scale = transform.scale;

		vertNum_ = 4;
		indexNum_ = 6;
		
		texturePath_ = "uvChecker.png";
	}
	void Update();

private:
	void Sprite2DVertexData();
	void Sprite2DIndexData();
};

