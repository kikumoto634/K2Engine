#pragma once
#include <list>
#include "Collider.h"

class CollisionManager
{
public:
	//リストのクリア
	void Reset(){
		colliders_.clear();
	}

	void CheckAllCollisions();

	void AddCollider(Collider* col)	{colliders_.push_back(col);}

private:
	void CheckCollisionPair(Collider* a, Collider* b);

private:
	std::list<Collider*> colliders_;
};

