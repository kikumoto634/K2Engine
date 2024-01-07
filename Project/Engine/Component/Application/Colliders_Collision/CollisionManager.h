#pragma once
#include <forward_list>
#include "Collider.h"

#include "Geometry/Sphere.h"

class CollisionManager
{
public:
	static CollisionManager* GetInstance();

public:
	void Update();
	void Draw(Camera* camera);

	//衝突判定
	void CheckAllCollisions();

	//コライダー追加
	inline void AddCollider(Collider* col)	{colliders_.push_front(col);}

	//コライダー削除
	inline void RemoveCollider(Collider* col)	{colliders_.remove(col);}

	//リストのクリア
	void Reset(){
		colliders_.clear();
	}

private:
	CollisionManager() = default;
	CollisionManager(const CollisionManager&)=delete;
	~CollisionManager() = default;
	CollisionManager& operator=(const CollisionManager&)=delete;

private:
	std::forward_list<Collider*> colliders_;
};

