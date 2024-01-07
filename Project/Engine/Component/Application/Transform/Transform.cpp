#include "Transform.h"

Transform::Transform():
	translate({0,0,0}),
	rotation({0,0,0}),
	scale({1,1,1})
{
	worldMatrix4x4_ = worldMatrix4x4_.MakeIdentityMatrix();
}

Transform::Transform(Vector3 trans, Vector3 rot, Vector3 scale):
	translate(trans),
	rotation(rot),
	scale(scale)
{
	worldMatrix4x4_ = worldMatrix4x4_.MakeIdentityMatrix();
}

Matrix4x4 Transform::GetWorldMatrix()
{
	worldMatrix4x4_ = 
		worldMatrix4x4_.MakeAffineMatrix(scale,rotation,translate);

	return worldMatrix4x4_;
}

Matrix4x4 Transform::GetScaleMatrix()
{
	Matrix4x4 matScale = MakeIdentityMatrix();
	matScale = MakeScaleMatrix(scale);
	return matScale;
}

Matrix4x4 Transform::GetRotMatrix()
{
	Matrix4x4 matRot = MakeIdentityMatrix();
	matRot *= MakeRotationZMatrix(rotation.z);
	matRot *= MakeRotationXMatrix(rotation.x);
	matRot *= MakeRotationYMatrix(rotation.y);

	return matRot;
}

Matrix4x4 Transform::GetTransMatrix()
{
	Matrix4x4 matTrans = MakeIdentityMatrix();
	matTrans = MakeTranslateMatrix(translate);
	return matTrans;
}
