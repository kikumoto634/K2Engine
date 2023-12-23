#include "CollisionManager.h"

void CollisionManager::CheckAllCollisions()
{
	using namespace std;
	list<Collider*>::iterator itrA = colliders_.begin();
	for(; itrA != colliders_.end(); ++itrA){
		Collider* a = *itrA;

		list<Collider*>::iterator itrB = itrA;
		itrB++;
		for(; itrB != colliders_.end(); ++itrB){
			Collider* b = *itrB;

			///ペア判定
			CheckCollisionPair(a,b);
		}
	}
}

void CollisionManager::CheckCollisionPair(Collider *a, Collider *b)
{
	Vector3 colAPos = a->GetCenterPos();
	Vector3 colBPos = b->GetCenterPos();
	float distance = Vector3(colBPos-colAPos).length();

	//Sphere to Sphere
	if(distance <= a->GetRadius() + b->GetRadius()){
		a->OnCollision();
		b->OnCollision();
	}
}
