#include "CollidersCheck.h"
#include "MathUtility.h"
#include <cmath>

bool CollidersCheck::CheckSphereToSphere(const Sphere_Collision &sphere1, const Sphere_Collision &sphere2, Vector3 *inter, Vector3* reject)
{
	// 中心点の距離の２乗 <= 半径の和の２乗　なら交差
	float dist = Vector3(sphere1.center - sphere2.center).length();

	float radius = sphere1.radius + sphere2.radius;
	radius *= radius;


	if(dist > radius)	{return false;}

	if(inter){
		//Aの半径が0の時、座標はBの中心、Bの半径が0の時座標はAの中心となる
		float t = sphere2.radius / (sphere1.radius + sphere2.radius);
		*inter = Lerp(sphere1.center, sphere2.center, t);
	}
	//押し出すベクトルを計算
	if(reject){
		float rejectLen = sphere1.radius + sphere2.radius - sqrtf(dist);
		*reject = Vector3(sphere1.center - sphere2.center).normalize();
		*reject *= rejectLen;
	}

	return true;
}
