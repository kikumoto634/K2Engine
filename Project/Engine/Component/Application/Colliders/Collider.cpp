#include "Collider.h"
#include "Geometry/SphereCollider.h"

void Collider::ColliderUpdate()
{
	if(!colliderModel){
		colliderModel = SphereCollider::Create();
		colliderModel->scale = {radius, radius, radius};
	}
}

void Collider::ColliderDraw(Vector3 pos, Matrix4x4 viewProjectionMatrix)
{
	colliderModel->translate = pos;
	colliderModel->Draw(viewProjectionMatrix);
}
