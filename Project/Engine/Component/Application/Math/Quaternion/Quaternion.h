#pragma once
class Quaternion
{
public:
	float x;
	float y;
	float z;
	float w;

public:
	Quaternion() {};
	Quaternion(float x, float y, float z, float w):
		x(x),
		y(y),
		z(z),
		w(w)
	{
	}
};

//積
Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs);
//単位Quaternion
Quaternion IdentityQuaternion();
//共役Quaternion
Quaternion Conjugate(const Quaternion& quaternion);
//Quaternionのnorm返却
float Norm(const Quaternion& quaternion);
//正規化Quaternion
Quaternion Normalize(const Quaternion& quaternion);
//逆Quaternion
Quaternion Inverse(const Quaternion& quaternion);

