#include "PostEffect.h"

PostEffect *PostEffect::Create(Transform transform)
{
	PostEffect* instance = new PostEffect(transform);
	instance->Initialize();
	instance->PostEffectVertexData();
	instance->PostEffectIndexData();
	return instance;
}

void PostEffect::Update()
{
}

void PostEffect::PostEffectVertexData()
{
	vertData_[0].position = {0.0f, size.y, 0.0f, 1.0f};		//左下
	vertData_[0].texcoord = {0.0f, 1.0f};
	vertData_[0].normal = {0.0f, 0.0f, -1.0f};
	vertData_[1].position = {0.0f, 0.0f, 0.0f, 1.0f};		//左上
	vertData_[1].texcoord = {0.0f, 0.0f};
	vertData_[1].normal = {0.0f, 0.0f, -1.0f};
	vertData_[2].position = {size.x, size.y, 0.0f, 1.0f};	//右下
	vertData_[2].texcoord = {1.0f, 1.0f};
	vertData_[2].normal = {0.0f, 0.0f, -1.0f};
	vertData_[3].position = {size.x, 0.0f, 0.0f, 1.0f};		//右上
	vertData_[3].texcoord = {1.0f, 0.0f};
	vertData_[3].normal = {0.0f, 0.0f, -1.0f};
}

void PostEffect::PostEffectIndexData()
{
	indexData_[0] = 0;	indexData_[1] = 1;	indexData_[2] = 2;
	indexData_[3] = 1;	indexData_[4] = 3;	indexData_[5] = 2;
}
