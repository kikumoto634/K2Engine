#include "CollisionManager.h"

void CollisionManager::Update()
{
	for(Collider* col : colliders_){
		col->ColliderUpdate();
	}
}

void CollisionManager::Draw(Camera* camera)
{
	for(Collider* col : colliders_){
		col->ColliderDraw(col->GetColliderCenterPos(), camera);
	}
}

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
	Vector3 colAPos = a->GetColliderCenterPos();
	Vector3 colBPos = b->GetColliderCenterPos();
	float distance = Vector3(colBPos-colAPos).length();

	//衝突フィルタリング
	if(a->GetCollisionAttribute() != b->GetCollisionMask() ||
		b->GetCollisionAttribute() != a->GetCollisionMask()){
		return;
	}

	//Sphere to Sphere
	if(distance <= a->GetRadius() + b->GetRadius()){
		a->OnCollision();
		b->OnCollision();
	}
}
