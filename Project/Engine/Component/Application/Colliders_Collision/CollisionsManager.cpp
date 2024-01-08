#include "CollisionsManager.h"
#include "CollisionCheck.h"
#include "CollisionInfo.h"

CollisionsManager* CollisionsManager::instance = nullptr;

CollisionsManager *CollisionsManager::GetInstance()
{
	if(!instance)instance = new CollisionsManager();
	return instance;
}

void CollisionsManager::CheckAllCollisions()
{
	using namespace std;
	forward_list<BaseCollider*>::iterator itrA = colliders_.begin();
	for(; itrA != colliders_.end(); ++itrA){
		BaseCollider* a = *itrA;

		forward_list<BaseCollider*>::iterator itrB = itrA;
		itrB++;
		for(; itrB != colliders_.end(); ++itrB){
			BaseCollider* b = *itrB;

			if(a->GetShapeType() == COLLISIONSHAPE_SPHERE &&
				b->GetShapeType() == COLLISIONSHAPE_SPHERE){
				SphereCP* SphereA = dynamic_cast<SphereCP*>(a);
				SphereCP* SphereB = dynamic_cast<SphereCP*>(b);
				Vector3 inter;
				if(CollisionCheck::CheckSphereToSphere(*SphereA, *SphereB, &inter)){
					a->OnCollision(CollisionInfo(b->GetGeometryBaseObject(), b, inter));
					b->OnCollision(CollisionInfo(a->GetGeometryBaseObject(), a, inter));
				}
			}
		}
	}
}
