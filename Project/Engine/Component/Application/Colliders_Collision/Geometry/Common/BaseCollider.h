#pragma once

/// <summary>
/// コライダーの基底
/// 
/// ・Managerで衝突判定をしなくてはいけないため、保有しているオブジェクトから座標、形状に沿った情報を常に受ける必要がある
/// （Playerなどには、変数 BaseColliderを保持させる。そこでSphereかAABBなのか new演算子で判断)
/// 
/// ・プラス、コライダーの形を変更させる必要があるため、Sphereなどの thisを継承したクラスでインスタンスを作成する必要がある
///
/// ・可視化用の描画も必要
/// 
/// ・Getter/Setterには属性たちを変更させるものが必要。
/// 
///  </summary>

#include "Camera.h"

#include "CollisionInfo.h"
#include "CollisionShapeTypa.h"

class GeometryBase;

class BaseCollider
{
public:
	BaseCollider() = default;
	virtual ~BaseCollider() = default;

	//保有クラスのOnCollisionを呼び出すため
	void Initialize(GeometryBase* object);
	virtual void Update(){};
	//可視化用
	virtual void ShapeDraw(Camera* camera){};

	//衝突時コールバック(Managerに受け渡す用)
	void OnCollision(CollisionInfo info);

	//Getter/Setter
	//ManagerでCollisionInfoに情報を受け渡すよう
	GeometryBase* GetGeometryBaseObject();
	CollisionShapeType GetShapeType()	{return shapeType_;}

	void SetShapeType(CollisionShapeType type)	{shapeType_ = type;}

protected:
	//コライダー形状
	CollisionShapeType shapeType_ = SHAPE_UNKNOWN;
private:
	//コライダーを持っているゲームオブジェクト
	GeometryBase* object_ = nullptr;
};

