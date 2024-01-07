#include "CollisionManager.h"
#include "CollisionCheck.h"

CollisionManager *CollisionManager::GetInstance()
{
	static CollisionManager instance;
	return &instance;
}

void CollisionManager::Update()
{
	for(Collider* col : colliders_){
		col->ColliderUpdate();
	}
}

void CollisionManager::Draw(Camera* camera)
{
	for(Collider* col : colliders_){
		col->ColliderDraw(col->GetObjects()->translate, camera);
	}
}

void CollisionManager::CheckAllCollisions()
{
	using namespace std;
	forward_list<Collider*>::iterator itrA = colliders_.begin();
	for(; itrA != colliders_.end(); ++itrA){
		Collider* a = *itrA;

		forward_list<Collider*>::iterator itrB = itrA;
		itrB++;
		for(; itrB != colliders_.end(); ++itrB){
			Collider* b = *itrB;

			if(a->GetShapeType() == COLLISIONSHAPE_SPHERE &&
				b->GetShapeType() == COLLISIONSHAPE_SPHERE){
				SphereCP* SphereA = dynamic_cast<SphereCP*>(a);
				SphereCP* SphereB = dynamic_cast<SphereCP*>(b);
				Vector3 inter;
				if(CollisionCheck::CheckSphereToSphere(*SphereA, *SphereB, &inter)){
					a->OnCollision(CollisionInfo(b->GetObjects(), b, inter));
					b->OnCollision(CollisionInfo(a->GetObjects(), a, inter));
				}
			}
		}
	}
}