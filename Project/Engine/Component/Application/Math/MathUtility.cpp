#include "MathUtility.h"
#include "Vector4.h"
#include "Easing.h"

#include <cmath>

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

float LerpShortAngle(float a, float b, float t)
{
	//角度差分
	float diff = b - a;

	//-2π~2π
	float temp1 = std::fmod(diff, 360.f * (3.141592f/180.f));
	//-1π~1π
	float temp2 = std::fmod(diff, 180.f*(3.141592f/180.f));
	
	if(temp1 > temp2){
		temp1 = temp2 - temp1;
	}
	return a + temp1*t;
}
