#pragma once
#include "Base/SpriteBase.h"
class Sprite2D : public SpriteBase
{
public:
	static Sprite2D* Create(std::string texturePath_ = "uvChecker.png" ,Transform transform = {{100,100,0}, {0,0,0}, {1,1,1}});

public:
	Sprite2D(std::string texturePath, Transform transform){
		translate = transform.translate;
		rotation = transform.rotation;
		scale = transform.scale;
		
		texturePath_ = texturePath;
	}
	void Update();

private:
	void Sprite2DVertexData();
	void Sprite2DIndexData();

	bool PipelineCreate() override;
};

