#pragma once
#include <list>
#include "Collider/BaseCollider.h"

class CollisionManager
{
public:
	static CollisionManager* GetInstance();

public:
	void CollisionAllCheck();

	void AddCollider(BaseCollider* col){
		colliders.push_back(col);
	}

private:
	void CollisionPairCheck(BaseCollider* a, BaseCollider* b);

private:
	static CollisionManager* instance;

private:
	std::list<BaseCollider*> colliders;
};

