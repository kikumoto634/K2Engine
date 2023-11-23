#include "Transform.h"

Transform::Transform():
	translate({0,0,0}),
	rotation({0,0,0}),
	scale({1,1,1})
{
	worldMatrix4x4 = worldMatrix4x4.MakeIdentityMatrix();
}

Transform::Transform(Vector3 trans, Vector3 rot, Vector3 scale):
	translate(trans),
	rotation(rot),
	scale(scale)
{
	worldMatrix4x4 = worldMatrix4x4.MakeIdentityMatrix();
}

Matrix4x4 Transform::GetWorldMatrix()
{
	worldMatrix4x4 = 
		worldMatrix4x4.MakeAffineMatrix(scale,DegreesToRadians(rotation),translate);

	return worldMatrix4x4;
}
