#pragma once
#include "Vector3.h"

class Collider
{
public:
	virtual ~Collider() = default;
	virtual void OnCollision(){};

	//Getter/Setter
	virtual Vector3 GetCenterPos()	const = 0;
	float GetRadius()	{return radius;}

	void SetRadius(float r)	{radius = r;}

private:
	float radius = 1.0f;
};

