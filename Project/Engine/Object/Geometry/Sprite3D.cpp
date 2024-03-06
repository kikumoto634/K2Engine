#include "Sprite3D.h"
#include <imgui.h>

Sprite3D *Sprite3D::Create(std::string texturePath_, Transform transform)
{
	Sprite3D* instance = new Sprite3D(texturePath_, transform);
	instance->Initialize();
	instance->Sprite3DVertexData();
	instance->Sprite3DIndexData();
	return instance;
}

void Sprite3D::Update()
{
#ifdef _DEBUG
	ImGui::Text("Sprite");
	ImGui::DragFloat3("Pos", &translate.x, 1.0f);
	ImGui::DragFloat3("Rot", &rotation.x, 1.0f);
	ImGui::DragFloat3("Scale", &scale.x, 1.0f);

	ImGui::DragFloat2("UVTranslate", &uvTransformSprite_.translate.x, 0.01f, -10.f, 10.f);
	ImGui::DragFloat2("UVScale", &uvTransformSprite_.scale.x, 0.01f,-10.f,10.f);
	ImGui::SliderAngle("UVRotate", &uvTransformSprite_.rotation.z);
#endif // _DEBUG
}

void Sprite3D::Sprite3DVertexData()
{
	vertData_[0].position = {-textureSize_.x/2, textureSize_.y/2, 0.0f, 1.0f};		//左上
	vertData_[0].texcoord = {0.0f, 0.0f};
	vertData_[0].normal = {0.0f, 0.0f, -1.0f};

	vertData_[1].position = {-textureSize_.x/2, -textureSize_.y/2, 0.0f, 1.0f};		//左下
	vertData_[1].texcoord = {0.0f, 1.0f};
	vertData_[1].normal = {0.0f, 0.0f, -1.0f};
	
	vertData_[2].position = {textureSize_.x/2, textureSize_.y/2, 0.0f, 1.0f};		//右上
	vertData_[2].texcoord = {1.0f, 0.0f};
	vertData_[2].normal = {0.0f, 0.0f, -1.0f};

	vertData_[3].position = {textureSize_.x/2, -textureSize_.y/2, 0.0f, 1.0f};	//右下
	vertData_[3].texcoord = {1.0f, 1.0f};
	vertData_[3].normal = {0.0f, 0.0f, -1.0f};

}

void Sprite3D::Sprite3DIndexData()
{
	indexData_[0] = 2;	indexData_[1] = 1;	indexData_[2] = 0;
	indexData_[3] = 2;	indexData_[4] = 3;	indexData_[5] = 1;
}

bool Sprite3D::PipelineCreate()
{
	return false;
}
