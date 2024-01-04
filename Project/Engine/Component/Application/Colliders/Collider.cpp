#include "Collider.h"
#include "Geometry/SphereCollider.h"

void Collider::ColliderUpdate()
{
	if(!colliderModel_){
		colliderModel_ = SphereCollider::Create();
		colliderModel_->scale = {radius_, radius_, radius_};
	}
}

void Collider::ColliderDraw(Vector3 pos, Matrix4x4 viewProjectionMatrix)
{
	colliderModel_->translate = pos;
	colliderModel_->Draw(viewProjectionMatrix);
}
