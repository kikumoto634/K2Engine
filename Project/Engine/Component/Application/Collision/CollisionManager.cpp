#include "CollisionManager.h"
#include "Collider/CollidersCheck.h"

CollisionManager* CollisionManager::instance_ = nullptr;

CollisionManager *CollisionManager::GetInstance()
{
	if(!instance_){
		instance_ = new CollisionManager();
	}
	return instance_;
}

void CollisionManager::CheckAllCollisions()
{
	std::forward_list<BaseCollider*>::iterator itA;
	std::forward_list<BaseCollider*>::iterator itB;

	//総当たり
	itA = colliders_.begin();
	for(; itA != colliders_.end(); ++itA){
		itB = itA;
		++itB;
		for(; itB != colliders_.end(); ++itB){
			
			//確認コライダー
			BaseCollider* colA = *itA;
			BaseCollider* colB = *itB;

			//タイプ確認
			//ともにSphere
			if(colA->GetShapeType() == COLLISIONSHAPE_SPHERE && 
				colB->GetShapeType() == COLLISIONSHAPE_SPHERE){
				Sphere_ColliderStruct* SphereA = dynamic_cast<Sphere_ColliderStruct*>(colA);
				Sphere_ColliderStruct* SphereB = dynamic_cast<Sphere_ColliderStruct*>(colB);

				Vector3 inter;
				if(CollidersCheck::CheckSphereToSphere(*SphereA, *SphereB, &inter)){
					colA->OnCollision(CollisionInfo(colB->GetObjObject(), colB, inter));
					colB->OnCollision(CollisionInfo(colA->GetObjObject(), colA, inter));
				}
			}
		}
	}
}

void CollisionManager::AddCollider(BaseCollider *collider)
{
	colliders_.push_front(collider);
}
