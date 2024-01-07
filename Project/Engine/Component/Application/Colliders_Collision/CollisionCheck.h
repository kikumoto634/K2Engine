#pragma once
#include "CollisionPrimitive.h"

class CollisionCheck
{
public:
	/// <summary>
	/// 球と平面の衝突判定
	/// </summary>
	/// <param name="sphere">球</param>
	/// <param name="plane">平面</param>
	/// <param name="inter">交点</param>
	static bool CheckSphere2Plane(const SphereCP& sphere, const PlaneCP& plane, Vector3* inter = nullptr);

	/// <summary>
	/// 点と三角形の最近設定を求める
	/// </summary>
	/// <param name="point">点</param>
	/// <param name="triangle">三角形</param>
	/// <param name="closest">最近接点(出力用)</param>
	static void ClosestPtPoint2Triangle(const Vector3& point, const TriangleCP& triangle, Vector3* closest);

	/// <summary>
	/// 球と法線付き三角形の当たりチェック
	/// </summary>
	/// <param name="sphere">球</param>
	/// <param name="triangle">三角形</param>
	/// <param name="inter">交点</param>
	static bool CheckSphere2Triangle(const SphereCP& sphere, const TriangleCP& triangle, Vector3* inter = nullptr, Vector3* reject = nullptr);

	/// <summary>
	/// 球と球の衝突判定
	/// </summary>
	/// <param name="sphere1">球</param>
	/// <param name="sphere2">球</param>
	/// <param name="inter">接触点</param>
	/// <param name="reject">押し出し</param>
	static bool CheckSphereToSphere(const SphereCP& sphere1,const SphereCP& sphere2, Vector3* inter = nullptr, Vector3* reject = nullptr);
};

