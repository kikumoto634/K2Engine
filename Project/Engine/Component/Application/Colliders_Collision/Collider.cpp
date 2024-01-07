#include "Collider.h"
#include "Geometry/SphereCollider.h"
#include "Geometry/Common/GeometryBase.h"

void Collider::ColliderUpdate()
{
	if(!colliderModel_){
		colliderModel_ = SphereCollider::Create();
		colliderModel_->scale = object_->scale;
	}
}

void Collider::ColliderDraw(Vector3 pos, Camera* camera)
{
	colliderModel_->translate = pos;
	colliderModel_->Draw(camera);
}
