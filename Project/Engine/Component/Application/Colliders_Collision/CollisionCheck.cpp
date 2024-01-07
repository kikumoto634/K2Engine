#include "CollisionCheck.h"
#include <cmath>
#include <MathUtility.h>

bool CollisionCheck::CheckSphere2Plane(const SphereCP &sphere, const PlaneCP &plane, Vector3 *inter)
{
	//球の中心と原点の距離
	float dist = sphere.center.dot(plane.normal) - plane.distance;
	
	if(fabs(dist) > sphere.radius) return false;

	//交点計算
	if(inter){
		*inter = -dist * plane.normal + sphere.center;
	}

	return true;
}

void CollisionCheck::ClosestPtPoint2Triangle(const Vector3 &point, const TriangleCP &triangle, Vector3 *closest)
{
	//pointがp0の外側の頂点領域の中にあるかどうかチェック
	Vector3 p0_p1 = triangle.p1 - triangle.p0;
	Vector3 p0_p2 = triangle.p2 - triangle.p0;
	Vector3 p0_pt = point - triangle.p0;

	float d1 = p0_p1.dot(p0_pt);
	float d2 = p0_p2.dot(p0_pt);

	if(d1 <= 0.0f && d2 <= 0.0f)
	{
		//p0は最近
		*closest = triangle.p0;
		return ;
	}

	//pointがp1の外側の頂点領域の中にあるかどうかチェック
	Vector3 p1_pt = point - triangle.p1;

	float d3 = p0_p1.dot(p1_pt);
	float d4 = p0_p2.dot(p1_pt);

	if(d3 >= 0.0f && d4 <= d3)
	{
		//p1が最近
		*closest = triangle.p1;
		return ;
	}

	//pointがp0_p1の編領域のな簡易あるかどうかチェックし、あればpointのp0_p1上に対する射影を返す
	float vc = d1 * d4 - d3 * d2;
	
	if(vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f)
	{
		float v = d1 / (d1 - d3);
		*closest = triangle.p0 + v * p0_p1;
		return ;
	}

	//pointがp2の外側の頂点領域の中にあるかどうかチェック
	Vector3 p2_pt = point - triangle.p2;

	float d5 = p0_p1.dot(p2_pt);
	float d6 = p0_p2.dot(p2_pt);
	
	if(d6 >= 0.0f && d5 <= d6)
	{
		*closest = triangle.p2;
		return ;
	}

	//pointがp0_p2の辺領域の中にあるかどうかチェックし、あればpointのp0_p2上に対する射影を返す
	float vb = d5 * d2 - d1 * d6;
	
	if(vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f)
	{
		float w = d2 / (d2 - d6);
		*closest = triangle.p0 + w * p0_p2;
		return ;
	}

	//pointがp1_p2の辺領域の中にあるかどうかチェックし、あればpointのp1_p2に対する射影を返す
	float va = d3 * d6 - d5 * d4;

	if(va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f)
	{
		float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
		*closest = triangle.p1 + w * (triangle.p2 - triangle.p1);
		return ;
	}


	float denom = 1.0f / (va + vb + vc);
	float v = vb * denom;
	float w = vc * denom;
	*closest = triangle.p0 + p0_p1 * v + p0_p2 * w;
}

bool CollisionCheck::CheckSphere2Triangle(const SphereCP &sphere, const TriangleCP &triangle, Vector3 *inter, Vector3* reject)
{
	Vector3 p;

	//球の中心に対する最近接点である三角形上にある点pを見つける
	ClosestPtPoint2Triangle(sphere.center, triangle, &p);
	//点pと球の中心の差分ベクトル
	Vector3 v = p - sphere.center;
	//距離の二乗を求める
	//(同じベクトル同士の内積は三平方の定理のルート内部の式と一致する)
	float temp = v.dot(v);
	//球と三角形の距離は半径イカなら当たっていない
	if(temp > sphere.radius * sphere.radius){
		return false;
	}
	//疑似交点を計算
	if(inter){
		//三角形上の最近接点pを疑似交点とする
		*inter = p;
	}
	//押し出すベクトルを計算
	if(reject){
		float ds = sphere.center.dot(triangle.normal);
		float dt = triangle.p0.dot(triangle.normal);
		float rejectLen = dt - ds + sphere.radius;
		*reject = triangle.normal * rejectLen;
	}

	return true;
}

bool CollisionCheck::CheckSphereToSphere(const SphereCP &sphere1, const SphereCP &sphere2, Vector3 *inter, Vector3 *reject)
{
	// 中心点の距離の２乗 <= 半径の和の２乗　なら交差
	float dist = sqrtf(Vector3(sphere1.center - sphere2.center).length());

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
