#include "ObjModel.h"
#include <imgui.h>
#include "SpriteLoader.h"

ObjModel *ObjModel::Create(std::string filePath, Transform transform)
{
	ObjModel *instance = new ObjModel(filePath, transform);
	instance->ObjModelLoad();
	instance->Initialize(false);
	instance->ObjModelVertexData();
	return instance;
}

void ObjModel::Update()
{
#ifdef _DEBUG
	ImGui::Text("Obj");
	ImGui::DragFloat3("Pos   - Obj", &transform.translate.x, 0.01f);
	ImGui::DragFloat3("Rot   - Obj", &transform.rotation.x, 0.01f);
	ImGui::DragFloat3("Scale - Obj", &transform.scale.x, 0.01f);
#endif // _DEBUG
}

void ObjModel::ObjModelLoad()
{
	//モデル読み込み
	modelData_ = LoadObjFile(filePath_);
	//頂点数決め
	vertNum_ = (UINT)modelData_.vertices.size();
	//画像パス
	texturePath_ = modelData_.material.textureFilePath;
	SpriteLoader::LoadTexture(DirectXCommon::GetInstance(), texturePath_);
}

void ObjModel::ObjModelVertexData()
{
	//データ送信
	std::memcpy(vertData_, modelData_.vertices.data(), sizeof(VertexData) * vertNum_);
}
