#include "BaseCollider.h"
#include "Geometry/Common/GeometryBase.h"

void BaseCollider::Initialize(GeometryBase *object)
{
	object_ = object;
}

void BaseCollider::OnCollision(CollisionInfo info)
{
	object_->OnCollision(info);
}

GeometryBase *BaseCollider::GetGeometryBaseObject()
{
	return object_;
}
