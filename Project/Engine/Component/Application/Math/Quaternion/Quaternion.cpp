#include "Quaternion.h"
#include <cmath>

Quaternion Multiply(const Quaternion &lhs, const Quaternion &rhs)
{
	Quaternion result;

	result.x = (lhs.y*rhs.z - lhs.z*rhs.y + rhs.w*lhs.x + lhs.w*rhs.x);
	result.y = (lhs.z*rhs.x - lhs.x*rhs.z + rhs.w*lhs.y + lhs.w*rhs.y);
	result.z = (lhs.x*rhs.y - lhs.y*rhs.x + rhs.w*lhs.z + lhs.w*rhs.z);
	result.w = (lhs.w*rhs.w - lhs.x*rhs.x - rhs.y*lhs.y - lhs.z*rhs.z);

	return result;
}

Quaternion IdentityQuaternion()
{
	Quaternion result = {0,0,0,1};
	return result;
}

Quaternion Conjugate(const Quaternion &quaternion)
{
	Quaternion result = {-quaternion.x, -quaternion.y, -quaternion.z, quaternion.w};
	return result;
}

float Norm(const Quaternion &quaternion)
{
	float result = powf(quaternion.x, 2) + powf(quaternion.y, 2) + powf(quaternion.z, 2) + powf(quaternion.w, 2);
	result = sqrtf(result);
	return result;
}

Quaternion Normalize(const Quaternion &quaternion)
{
	Quaternion result = quaternion;
	result.x /= Norm(quaternion);
	result.y /= Norm(quaternion);
	result.z /= Norm(quaternion);
	result.w /= Norm(quaternion);
	return result;
}

Quaternion Inverse(const Quaternion &quaternion)
{
	float norm = powf(quaternion.x, 2) + powf(quaternion.y, 2) + powf(quaternion.z, 2) + powf(quaternion.w, 2);
	
	float invNorm = 1.0f/norm;

	Quaternion result = {
		-quaternion.x*invNorm,
		-quaternion.y*invNorm,
		-quaternion.z*invNorm,
		quaternion.w*invNorm,
	};

	return result;
}
