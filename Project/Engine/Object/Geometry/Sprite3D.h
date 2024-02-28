#pragma once
#include "Base/Sprite3DBase.h"
class Sprite3D : public Sprite3DBase
{
public:
	static Sprite3D* Create(std::string texturePath_,Transform transform = {{0,0,0}, {0,0,0}, {1,1,1}});

public:
	Sprite3D(std::string texturePath, Transform transform){
		translate = transform.translate;
		rotation = transform.rotation;
		scale = transform.scale;
		
		texturePath_ = texturePath;
	}
	void Update();

	//Getter
	Vector2 GetTexSize()	{return textureSize_;}

private:
	void Sprite3DVertexData();
	void Sprite3DIndexData();

	bool PipelineCreate() override;
};

