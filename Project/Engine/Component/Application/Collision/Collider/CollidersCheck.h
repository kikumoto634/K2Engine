#pragma once
#include "Type/Sphere-CollisionStruct.h"

class CollidersCheck
{
public:
	/// <summary>
	/// 弾同士の接触
	/// </summary>
	/// <param name="sphere1">球1</param>
	/// <param name="sphere2">球2</param>
	/// <param name="inter">接触点</param>
	/// <returns>接触しているか</returns>
	static bool CheckSphereToSphere(const Sphere_Collision& sphere1, const Sphere_Collision& sphere2, Vector3* inter = nullptr, Vector3* reject = nullptr);
};

