#pragma once
#include <list>
#include "Collider.h"

#include "Geometry/Sphere.h"

class CollisionManager
{
public:
	void Update();
	void Draw(Camera* camera);
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

