#include "MathUtility.h"
#include "Vector4.h"

Vector3 Multiplication(const Vector3 &v, const Matrix4x4 &m)
{
   Vector4 result{
		v.x*m.m[0][0] + v.y*m.m[1][0] + v.z*m.m[2][0] + m.m[3][0],
		v.x*m.m[0][1] + v.y*m.m[1][1] + v.z*m.m[2][1] + m.m[3][1],
		v.x*m.m[0][2] + v.y*m.m[1][2] + v.z*m.m[2][2] + m.m[3][2],
		// ベクトルを4次元に拡張し、行列の最後の列を考慮に入れる
		v.x*m.m[0][3] + v.y*m.m[1][3] + v.z*m.m[2][3] + m.m[3][3]
	};

	// 4次元ベクトルから3次元ベクトルに変換
    return Vector3{result.x / result.w, result.y / result.w, result.z / result.w}; 
}
