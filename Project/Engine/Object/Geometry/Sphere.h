#pragma once
#include "Common/GeometryBase.h"
class Sphere : public GeometryBase
{
public:
	static Sphere* Create();

public:
	Sphere(){
		//頂点数決め
		vertNum_ = kSubdivision*kSubdivision*6;
	}
	void Update();

	//Getter
	Transform GetTransform()	{return transform_;}
	Vector4 GetColor()			{return color_;}
	bool GetLightEnable()		{return isLightEnable;}

	Vector4 GetLightColor()		{return lightColor_;}
	Vector3 GetLightDirection()	{return lightDirection_;}
	float GetLightIntensity()	{return lightIntensity;}

	//Setter
	void SetTransform(Transform trans)	{transform_ = trans;}

	void SetLightDirection(Vector3 dir)	{lightDirection_ = dir;}

private:
	void SphereVertexData();

private:
	//球体分割数
	const uint32_t kSubdivision = 16;
};

