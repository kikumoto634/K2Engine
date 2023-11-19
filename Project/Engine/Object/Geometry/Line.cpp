#include "Line.h"
#include <imgui.h>

Line *Line::Create(Vector3 start, Vector3 end)
{
	Line* instance = new Line(start, end);
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

	ImGui::DragFloat3("Target", &target.x, 0.01f);


	//長さ
	Vector3 sub = target - translate;
	scale.x = sub.length();

	//X軸とY軸に対する角度
	float horizontal = std::atan2f(sub.y, sub.x);
	//sub.y = 0.f;
	//3D空間内の角度
	float vertical = std::atan2f(sub.z, sub.length());

	rotation = {0, horizontal, vertical};
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
