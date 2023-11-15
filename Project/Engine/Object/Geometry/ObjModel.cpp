#include "ObjModel.h"
#include <imgui.h>


ObjModel *ObjModel::Create(std::string filePath, Transform transform)
{
	ObjModel* instance = new ObjModel(filePath, transform);
	instance->ObjModelLoad();
	instance->Initialize(false);
	instance->ObjModelVertexData();
	return instance;
}

void ObjModel::Update()
{
	ImGui::Text("Obj");
	ImGui::DragFloat3("Pos", &translate.x, 0.01f);
	ImGui::DragFloat3("Rot", &rotation.x, 0.01f);
	ImGui::DragFloat3("Scale", &scale.x, 0.01f);
}

void ObjModel::ObjModelLoad()
{
	//モデル読み込み
	modelData_ = LoadObjFile(filePath_);
	//頂点数決め
	vertNum_ = (UINT)modelData_.vertices.size();
	//画像パス
	texturePath_ = modelData_.material.textureFilePath;
}

void ObjModel::ObjModelVertexData()
{
	//データ送信
	std::memcpy(vertData_, modelData_.vertices.data(), sizeof(VertexData)*vertNum_);
}
