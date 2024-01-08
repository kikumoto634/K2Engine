#include "SphereCollider.h"
#include "Geometry/Common/GeometryBase.h"

SphereCollider *SphereCollider::Create(GeometryBase* object, Vector3 offset, float radius)
{
	SphereCollider* instance = new SphereCollider(offset, radius);
	instance->Initialize(object);
	instance->ShapeInitialize();
	instance->VertexData();
	instance->IndexData();
	return instance;
}

void SphereCollider::Update()
{
	SphereCP::center = GetGeometryBaseObject()->translate + offset_;
	SphereCP::radius = radius_;

	translate = GetGeometryBaseObject()->translate;
}

void SphereCollider::ShapeDraw(Camera *camera)
{
	BaseShapeCollider::ShapeBaseDraw(camera);
}

void SphereCollider::VertexData()
{
	//球体
	//PI円周率
	const float pi = 3.14159265f;
	//経度分割1つ分の角度 φ
	const float kLonEvery = pi * 2.0f / (float)kSubdivision_;
	//緯度分割1つ分の角度 Θ
	const float kLatEvert = pi / (float)kSubdivision_;

	//経度インデックス
	uint32_t lonIndex = 0;
	//緯度インデックス
	uint32_t latIndex = 0;

	//緯度の方向に分割
	for(latIndex = 0; latIndex < kSubdivision_; ++latIndex){
		float lat = -pi / 2.0f + kLatEvert * latIndex;	//Θ
		//経度の方向に分割しながら絵を書く
		for(lonIndex = 0; lonIndex < kSubdivision_; ++lonIndex){
			uint32_t start = (latIndex * kSubdivision_ + lonIndex) * 4;
			float lon = lonIndex * kLonEvery;	//φ

			float u = (float)lonIndex / (float)kSubdivision_;
			float v = 1.0f - (float)latIndex / (float)kSubdivision_;
			float uvLength = 1.0f / (float)kSubdivision_;

			//頂点データを入力
			vertData_[start].x = cos(lat) * cos(lon);
			vertData_[start].y = sin(lat);
			vertData_[start].z = cos(lat) * sin(lon);
			vertData_[start].w = 1.0f;
			
			vertData_[start+1].x = cos(lat+kLatEvert) * cos(lon);
			vertData_[start+1].y = sin(lat+kLatEvert);
			vertData_[start+1].z = cos(lat+kLatEvert) * sin(lon);
			vertData_[start+1].w = 1.0f;

			vertData_[start+2].x = cos(lat) * cos(lon+kLonEvery);
			vertData_[start+2].y = sin(lat);
			vertData_[start+2].z = cos(lat) * sin(lon+kLonEvery);
			vertData_[start+2].w = 1.0f;

			
			vertData_[start+3].x = cos(lat+kLatEvert) * cos(lon+kLonEvery);
			vertData_[start+3].y = sin(lat+kLatEvert);
			vertData_[start+3].z = cos(lat+kLatEvert) * sin(lon+kLonEvery);
			vertData_[start+3].w = 1.0f;
		}
	}
}

void SphereCollider::IndexData()
{
	uint32_t lonIndex = 0;
	uint32_t latIndex = 0;
	for(latIndex = 0; latIndex < kSubdivision_; ++latIndex){
		for(lonIndex = 0; lonIndex < kSubdivision_; ++lonIndex){
			uint32_t start = (latIndex * kSubdivision_ + lonIndex) * 6;
			uint32_t vertStart = (latIndex * kSubdivision_ + lonIndex) * 4;

			//インデックス
			indexData_[start] = vertStart;		indexData_[start+1] = vertStart+1;	indexData_[start+2] = vertStart+2;
			indexData_[start+3] = vertStart+1;	indexData_[start+4] = vertStart+3;	indexData_[start+5] = vertStart+2;
		}
	}
}