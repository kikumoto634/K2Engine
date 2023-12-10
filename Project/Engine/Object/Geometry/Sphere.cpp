#include "Sphere.h"
#include <imgui.h>

Sphere *Sphere::Create(Transform transform)
{
	Sphere* instance = new Sphere(transform);
	instance->Initialize();
	instance->SphereVertexData();
	instance->SphereIndexData();
	return instance;
}

void Sphere::Update()
{
#ifdef _DEBUG
	ImGui::Text("Sphere");
	ImGui::DragFloat3("Pos   - Sphere", &translate.x, 0.01f);
	ImGui::DragFloat3("Rot   - Sphere", &rotation.x, 1.f);
	ImGui::DragFloat3("Scale - Sphere", &scale.x, 0.01f);
#endif // _DEBUG
}

void Sphere::SphereVertexData()
{
	//球体
	//PI円周率
	const float pi = 3.14159265f;
	//経度分割1つ分の角度 φ
	const float kLonEvery = pi * 2.0f / (float)kSubdivision;
	//緯度分割1つ分の角度 Θ
	const float kLatEvert = pi / (float)kSubdivision;

	//経度インデックス
	uint32_t lonIndex = 0;
	//緯度インデックス
	uint32_t latIndex = 0;

	//緯度の方向に分割
	for(latIndex = 0; latIndex < kSubdivision; ++latIndex){
		float lat = -pi / 2.0f + kLatEvert * latIndex;	//Θ
		//経度の方向に分割しながら絵を書く
		for(lonIndex = 0; lonIndex < kSubdivision; ++lonIndex){
			uint32_t start = (latIndex * kSubdivision + lonIndex) * 4;
			float lon = lonIndex * kLonEvery;	//φ

			float u = (float)lonIndex / (float)kSubdivision;
			float v = 1.0f - (float)latIndex / (float)kSubdivision;
			float uvLength = 1.0f / (float)kSubdivision;

			//頂点データを入力
			vertData_[start].position.x = cos(lat) * cos(lon);
			vertData_[start].position.y = sin(lat);
			vertData_[start].position.z = cos(lat) * sin(lon);
			vertData_[start].position.w = 1.0f;
			vertData_[start].texcoord.x = u;
			vertData_[start].texcoord.y = v;
			vertData_[start].normal.x = vertData_[start].position.x;
			vertData_[start].normal.y = vertData_[start].position.y;
			vertData_[start].normal.z = vertData_[start].position.z;
			
			vertData_[start+1].position.x = cos(lat+kLatEvert) * cos(lon);
			vertData_[start+1].position.y = sin(lat+kLatEvert);
			vertData_[start+1].position.z = cos(lat+kLatEvert) * sin(lon);
			vertData_[start+1].position.w = 1.0f;
			vertData_[start+1].texcoord.x = u;
			vertData_[start+1].texcoord.y = v - uvLength;
			vertData_[start+1].normal.x = vertData_[start+1].position.x;
			vertData_[start+1].normal.y = vertData_[start+1].position.y;
			vertData_[start+1].normal.z = vertData_[start+1].position.z;

			vertData_[start+2].position.x = cos(lat) * cos(lon+kLonEvery);
			vertData_[start+2].position.y = sin(lat);
			vertData_[start+2].position.z = cos(lat) * sin(lon+kLonEvery);
			vertData_[start+2].position.w = 1.0f;
			vertData_[start+2].texcoord.x = u + uvLength;
			vertData_[start+2].texcoord.y = v;
			vertData_[start+2].normal.x = vertData_[start+2].position.x;
			vertData_[start+2].normal.y = vertData_[start+2].position.y;
			vertData_[start+2].normal.z = vertData_[start+2].position.z;

			
			vertData_[start+3].position.x = cos(lat+kLatEvert) * cos(lon+kLonEvery);
			vertData_[start+3].position.y = sin(lat+kLatEvert);
			vertData_[start+3].position.z = cos(lat+kLatEvert) * sin(lon+kLonEvery);
			vertData_[start+3].position.w = 1.0f;
			vertData_[start+3].texcoord.x = u + uvLength;
			vertData_[start+3].texcoord.y = v - uvLength;
			vertData_[start+3].normal.x = vertData_[start+3].position.x;
			vertData_[start+3].normal.y = vertData_[start+3].position.y;
			vertData_[start+3].normal.z = vertData_[start+3].position.z;

		}
	}
}

void Sphere::SphereIndexData()
{
	uint32_t lonIndex = 0;
	uint32_t latIndex = 0;
	for(latIndex = 0; latIndex < kSubdivision; ++latIndex){
		for(lonIndex = 0; lonIndex < kSubdivision; ++lonIndex){
			uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
			uint32_t vertStart = (latIndex * kSubdivision + lonIndex) * 4;

			//インデックス
			indexData_[start] = vertStart;		indexData_[start+1] = vertStart+1;	indexData_[start+2] = vertStart+2;
			indexData_[start+3] = vertStart+1;	indexData_[start+4] = vertStart+3;	indexData_[start+5] = vertStart+2;
		}
	}
}

