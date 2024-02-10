#include "TestObj.h"
#include <imgui.h>
#include "SpriteLoader.h"

TestObj *TestObj::Create(std::string filePath, Transform transform, BlendSetting::BlendMode mode)
{
	TestObj *instance = new TestObj(filePath, transform,mode);
	instance->ObjModelLoad();
	instance->Initialize(false);
	instance->ObjModelVertexData();
	return instance;
}

void TestObj::ObjModelLoad()
{
	//モデル読み込み
	modelData_ = LoadObjFile(filePath_);
	//頂点数決め
	vertNum_ = (UINT)modelData_.vertices.size();
	//画像パス
	texturePath_ = modelData_.material.textureFilePath;
	SpriteLoader::LoadTexture(DirectXCommon::GetInstance(), texturePath_);
}

void TestObj::ObjModelVertexData()
{
	//データ送信
	std::memcpy(vertData_, modelData_.vertices.data(), sizeof(VertexData) * vertNum_);
}

bool TestObj::PipelineCreate()
{
	return false;
}
