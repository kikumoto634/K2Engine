#include "Line.h"
#include <imgui.h>

Line *Line::Create(Transform transform)
{
	Line* instance = new Line(transform);
	instance->Initialize(false);
	instance->LineVertexData();
	return instance;
}

void Line::Update()
{
	ImGui::Text("Line");
	ImGui::DragFloat3("Pos", &translate.x, 0.01f);
	ImGui::DragFloat3("Rot", &rotation.x, 0.01f);
	ImGui::DragFloat3("Scale", &scale.x, 0.01f);
}

void Line::LineVertexData()
{
	vertData_[0].position = {0,0,0,1};
	vertData_[0].normal = {0,0,0};
	vertData_[0].texcoord = {0,0};

	vertData_[1].position = {1,0,0,1};
	vertData_[1].normal = {0,0,0};
	vertData_[1].texcoord = {0,0};
}
