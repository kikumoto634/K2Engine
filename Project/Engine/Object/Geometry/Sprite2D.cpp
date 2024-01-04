#include "Sprite2D.h"
#include <imgui.h>

Sprite2D *Sprite2D::Create(Transform transform)
{
	Sprite2D* instance = new Sprite2D(transform);
	instance->Initialize();
	instance->Sprite2DVertexData();
	instance->Sprite2DIndexData();
	return instance;
}

void Sprite2D::Update()
{
	ImGui::Text("Sprite");
	ImGui::DragFloat3("Pos", &translate.x, 1.0f);
	ImGui::DragFloat3("Rot", &rotation.x, 1.0f);
	ImGui::DragFloat3("Scale", &scale.x, 1.0f);

	ImGui::DragFloat2("UVTranslate", &uvTransformSprite_.translate.x, 0.01f, -10.f, 10.f);
	ImGui::DragFloat2("UVScale", &uvTransformSprite_.scale.x, 0.01f,-10.f,10.f);
	ImGui::SliderAngle("UVRotate", &uvTransformSprite_.rotation.z);
}

void Sprite2D::Sprite2DVertexData()
{
	vertData_[0].position = {0.0f, textureSize_.y, 0.0f, 1.0f};		//左下
	vertData_[0].texcoord = {0.0f, 1.0f};
	vertData_[0].normal = {0.0f, 0.0f, -1.0f};

	vertData_[1].position = {0.0f, 0.0f, 0.0f, 1.0f};		//左上
	vertData_[1].texcoord = {0.0f, 0.0f};
	vertData_[1].normal = {0.0f, 0.0f, -1.0f};

	vertData_[2].position = {textureSize_.x, textureSize_.y, 0.0f, 1.0f};	//右下
	vertData_[2].texcoord = {1.0f, 1.0f};
	vertData_[2].normal = {0.0f, 0.0f, -1.0f};

	vertData_[3].position = {textureSize_.x, 0.0f, 0.0f, 1.0f};		//右上
	vertData_[3].texcoord = {1.0f, 0.0f};
	vertData_[3].normal = {0.0f, 0.0f, -1.0f};
}

void Sprite2D::Sprite2DIndexData()
{
	indexData_[0] = 0;	indexData_[1] = 1;	indexData_[2] = 2;
	indexData_[3] = 1;	indexData_[4] = 3;	indexData_[5] = 2;
}
