#include "CollisionManager.h"

CollisionManager* CollisionManager::instance = nullptr;
using namespace std;

CollisionManager *CollisionManager::GetInstance()
{
	if(!instance){
		instance = new CollisionManager();
	}
	return instance;
}

void CollisionManager::CollisionAllCheck()
{
	list<BaseCollider*>::iterator itrA = colliders.begin();
	for(;itrA != colliders.end(); ++itrA){
		BaseCollider* a = *itrA;

		list<BaseCollider*>::iterator itrB = itrA;
		itrB++;
		for(; itrB != colliders.end(); ++itrB){
			BaseCollider* b = *itrB;

			CollisionPairCheck(a,b);
		}
	}
}

void CollisionManager::CollisionPairCheck(BaseCollider *a, BaseCollider *b)
{
	if(a->GetCollisionAttribute() != b->GetCollisionMask() ||
		b->GetCollisionAttribute() != a->GetCollisionMask()) return;

	if(powf(b->GetCenter().x-a->GetCenter().x, 2)+powf(b->GetCenter().y-a->GetCenter().y, 2)+powf(b->GetCenter().z-a->GetCenter().z, 2) <= powf(a->GetR()+b->GetR(),2)){
		a->OnCollision();
		b->OnCollision();
	}
}
