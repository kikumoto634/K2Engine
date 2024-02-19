#include "Matrix4x4.h"
#include <cmath>


Matrix4x4::Matrix4x4()
{
	m[0][0] = 0;
	m[0][1] = 0;
	m[0][2] = 0;
	m[0][3] = 0;
	
	m[1][0] = 0;
	m[1][1] = 0;
	m[1][2] = 0;
	m[1][3] = 0;
	
	m[2][0] = 0;
	m[2][1] = 0;
	m[2][2] = 0;
	m[2][3] = 0;

	m[3][0] = 0;
	m[3][1] = 0;
	m[3][2] = 0;
	m[3][3] = 0;
}

Matrix4x4::Matrix4x4(float m00, float m01, float m02, float m03, 
				     float m10, float m11, float m12, float m13, 
					 float m20, float m21, float m22, float m23, 
					 float m30, float m31, float m32, float m33)
{
	m[0][0] = m00;
	m[0][1] = m01;
	m[0][2] = m02;
	m[0][3] = m03;
	
	m[1][0] = m10;
	m[1][1] = m11;
	m[1][2] = m12;
	m[1][3] = m13;
	
	m[2][0] = m20;
	m[2][1] = m21;
	m[2][2] = m22;
	m[2][3] = m23;

	m[3][0] = m30;
	m[3][1] = m31;
	m[3][2] = m32;
	m[3][3] = m33;
}

Matrix4x4 Matrix4x4::MakeIdentityMatrix()
{
	Matrix4x4 result{
		1.0f, 0.0f, 0.0f, 0.0f, 
		0.0f, 1.0f, 0.0f, 0.0f,
	    0.0f, 0.0f, 1.0f, 0.0f, 
		0.0f, 0.0f, 0.0f, 1.0f};

	return result;
}

Matrix4x4 Matrix4x4::MakeScaleMatrix(const Vector3& scale) 
{ 
	Matrix4x4 result;

	result = {
		scale.x, 0.0f,	  0.0f,		0.0f,
		0.0f,	 scale.y, 0.0f,		0.0f,
		0.0f,	 0.0f,	  scale.z,  0.0f,
		0.0f,	 0.0f,	  0.0f,		1.0f,
	};

	return result; 
}

Matrix4x4 Matrix4x4::MakeRotationXMatrix(float theta) 
{ 
	float sin = std::sin(theta);
	float cos = std::cos(theta);

	Matrix4x4 result = {
		1.0f,	0.0f,		0.0f,	0.0f,
		0.0f,	cos,		sin,	0.0f,
		0.0f,	-sin,		cos,	0.0f,
		0.0f,	0.0f,		0.0f,	1.0f,
	};

	return result; 
}

Matrix4x4 Matrix4x4::MakeRotationYMatrix(float theta) {
	float sin = std::sin(theta);
	float cos = std::cos(theta);

	Matrix4x4 result = {
		cos, 0.0f, -sin, 0.0f, 
		0.0f,1.0f, 0.0f, 0.0f,
		sin, 0.0f, cos,  0.0f, 
		0.0f,0.0f, 0.0f, 1.0f
	};

	return result;
}

Matrix4x4 Matrix4x4::MakeRotationZMatrix(float theta) {
	float sin = std::sin(theta);
	float cos = std::cos(theta);

	Matrix4x4 result = {
		cos,  sin,  0.0f, 0.0f, 
		-sin, cos,  0.0f, 0.0f,
	    0.0f, 0.0f, 1.0f, 0.0f, 
		0.0f, 0.0f, 0.0f, 1.0f
	};

	return result;
}

Matrix4x4 Matrix4x4::MakeTranslateMatrix(const Vector3& trans) 
{ 
	Matrix4x4 result = {
		1.0f,	 0.0f,	  0.0f,	   0.0f,
		0.0f,	 1.0f,	  0.0f,	   0.0f,
		0.0f,	 0.0f,	  1.0f,	   0.0f,
		trans.x, trans.y, trans.z, 1.0f,
	};

	return result; 
}

Matrix4x4 Matrix4x4::MakeAffineMatrix(const Vector3& scale, const Vector3& rot, const Vector3& trans) {
	
	//スケーリング行列
	Matrix4x4 matScale = MakeScaleMatrix(scale);

	//回転行列
	Matrix4x4 matRotX = MakeRotationXMatrix(rot.x);
	Matrix4x4 matRotY = MakeRotationYMatrix(rot.y);
	Matrix4x4 matRotZ = MakeRotationZMatrix(rot.z);
	//合成(Z * X * Y)
	Matrix4x4 matRot = matRotZ * matRotX * matRotY;

	//平行移動行列
	Matrix4x4 matTrans = MakeTranslateMatrix(trans);

	//合成
	Matrix4x4 result = matScale*matRot*matTrans;

	return result;
}


float Matrix4x4::Det(Matrix4x4 m) 
{ 
	float det = m.m[0][0]*m.m[1][1]*m.m[2][2]*m.m[3][3] + m.m[0][0]*m.m[1][2]*m.m[2][3]*m.m[3][1] + m.m[0][0]*m.m[1][3]*m.m[2][1]*m.m[3][2]
			  - m.m[0][0]*m.m[1][3]*m.m[2][2]*m.m[3][1] - m.m[0][0]*m.m[1][2]*m.m[2][1]*m.m[3][3] - m.m[0][0]*m.m[1][1]*m.m[2][3]*m.m[3][2]
			  - m.m[0][1]*m.m[1][0]*m.m[2][2]*m.m[3][3] - m.m[0][2]*m.m[1][0]*m.m[2][3]*m.m[3][1] - m.m[0][3]*m.m[1][0]*m.m[2][1]*m.m[3][2]
			  + m.m[3][0]*m.m[1][0]*m.m[2][2]*m.m[3][1] + m.m[0][2]*m.m[1][0]*m.m[2][1]*m.m[3][3] + m.m[0][1]*m.m[1][0]*m.m[2][3]*m.m[3][2]
			  + m.m[0][1]*m.m[1][2]*m.m[2][0]*m.m[3][3] + m.m[0][2]*m.m[1][3]*m.m[2][0]*m.m[3][1] + m.m[0][3]*m.m[1][1]*m.m[2][0]*m.m[3][2]
			  - m.m[0][3]*m.m[1][2]*m.m[2][0]*m.m[3][1] - m.m[0][2]*m.m[1][1]*m.m[2][0]*m.m[3][3] - m.m[0][1]*m.m[1][3]*m.m[2][0]*m.m[3][2]
			  - m.m[0][1]*m.m[1][2]*m.m[2][3]*m.m[3][0] - m.m[0][2]*m.m[1][3]*m.m[2][1]*m.m[3][0] - m.m[0][3]*m.m[1][1]*m.m[2][2]*m.m[3][0]
			  + m.m[0][3]*m.m[1][2]*m.m[2][1]*m.m[3][0] + m.m[0][2]*m.m[1][1]*m.m[2][3]*m.m[3][0] + m.m[0][1]*m.m[1][3]*m.m[2][2]*m.m[3][0];
	return det; 
}

Matrix4x4 Matrix4x4::Inverse(Matrix4x4 m) {
	

	float invDet = 1.f/Det(m);

	Matrix4x4 result = {};
	float inv[16];
	inv[0]  = invDet*( m.m[1][1]*m.m[2][2]*m.m[3][3]+m.m[1][2]*m.m[2][3]*m.m[3][1]+m.m[1][3]*m.m[2][1]*m.m[3][2]
					-  m.m[1][3]*m.m[2][2]*m.m[3][1]-m.m[1][2]*m.m[2][1]*m.m[3][3]-m.m[1][1]*m.m[2][3]*m.m[3][2]);
	inv[1]  = invDet*(-m.m[0][1]*m.m[2][2]*m.m[3][3]-m.m[0][2]*m.m[2][3]*m.m[3][1]-m.m[0][3]*m.m[2][1]*m.m[3][2]
					+  m.m[0][3]*m.m[2][2]*m.m[3][1]+m.m[0][2]*m.m[2][1]*m.m[3][3]+m.m[0][1]*m.m[2][3]*m.m[3][2]);
	inv[2]  = invDet*( m.m[0][1]*m.m[1][2]*m.m[3][3]+m.m[0][2]*m.m[1][3]*m.m[3][1]+m.m[0][3]*m.m[1][1]*m.m[3][2]
					-  m.m[0][3]*m.m[1][2]*m.m[3][1]-m.m[0][2]*m.m[1][1]*m.m[3][3]-m.m[0][1]*m.m[1][3]*m.m[3][2]);
	inv[3]  = invDet*(-m.m[0][1]*m.m[1][2]*m.m[2][3]-m.m[0][2]*m.m[1][3]*m.m[2][1]-m.m[0][3]*m.m[1][1]*m.m[2][2]
					+  m.m[0][3]*m.m[1][2]*m.m[2][1]+m.m[0][2]*m.m[1][1]*m.m[2][3]+m.m[0][1]*m.m[1][3]*m.m[2][2]);

	inv[4]  = invDet*(-m.m[1][0]*m.m[2][2]*m.m[3][3]-m.m[1][2]*m.m[2][3]*m.m[3][0]-m.m[1][3]*m.m[2][0]*m.m[3][2]
					+  m.m[1][3]*m.m[2][2]*m.m[3][0]+m.m[1][2]*m.m[2][0]*m.m[3][3]+m.m[1][0]*m.m[2][3]*m.m[3][2]);
	inv[5]  = invDet*( m.m[0][0]*m.m[2][2]*m.m[3][3]+m.m[0][2]*m.m[2][3]*m.m[3][0]+m.m[0][3]*m.m[2][0]*m.m[3][2]
					-  m.m[0][3]*m.m[2][2]*m.m[3][0]-m.m[0][2]*m.m[2][0]*m.m[3][3]-m.m[0][0]*m.m[2][3]*m.m[3][2]);
	inv[6]  = invDet*(-m.m[0][0]*m.m[1][2]*m.m[3][3]-m.m[0][2]*m.m[1][3]*m.m[3][0]-m.m[0][3]*m.m[1][0]*m.m[2][3]
					+  m.m[0][3]*m.m[1][2]*m.m[3][0]+m.m[0][2]*m.m[1][0]*m.m[3][3]+m.m[0][0]*m.m[1][3]*m.m[3][2]);
	inv[7]  = invDet*( m.m[0][0]*m.m[1][2]*m.m[2][3]+m.m[0][2]*m.m[1][3]*m.m[2][0]+m.m[0][3]*m.m[1][0]*m.m[2][2]
					-  m.m[0][3]*m.m[1][2]*m.m[2][0]-m.m[0][2]*m.m[1][0]*m.m[2][3]-m.m[0][0]*m.m[1][3]*m.m[2][2]);

	inv[8]  = invDet*( m.m[1][0]*m.m[2][1]*m.m[3][3]+m.m[1][1]*m.m[2][3]*m.m[3][0]+m.m[1][3]*m.m[2][0]*m.m[3][1]
					-  m.m[1][3]*m.m[2][1]*m.m[3][0]-m.m[1][1]*m.m[2][0]*m.m[3][3]-m.m[1][0]*m.m[2][3]*m.m[3][1]);
	inv[9]  = invDet*(-m.m[0][0]*m.m[2][1]*m.m[3][3]-m.m[0][1]*m.m[2][3]*m.m[3][0]-m.m[0][3]*m.m[2][0]*m.m[3][1]
					+  m.m[0][3]*m.m[2][1]*m.m[3][0]+m.m[0][1]*m.m[2][0]*m.m[3][3]+m.m[0][0]*m.m[2][3]*m.m[3][1]);
	inv[10] = invDet*( m.m[0][0]*m.m[1][1]*m.m[3][3]+m.m[0][1]*m.m[1][3]*m.m[3][0]+m.m[0][3]*m.m[1][0]*m.m[3][1]
					-  m.m[0][3]*m.m[1][1]*m.m[3][0]-m.m[0][1]*m.m[1][0]*m.m[3][3]-m.m[0][0]*m.m[1][3]*m.m[3][1]);
	inv[11] = invDet*(-m.m[0][0]*m.m[1][1]*m.m[2][3]-m.m[0][1]*m.m[1][3]*m.m[2][0]-m.m[0][3]*m.m[1][0]*m.m[2][1]
					+  m.m[0][3]*m.m[1][1]*m.m[2][0]+m.m[0][1]*m.m[1][0]*m.m[2][3]+m.m[0][0]*m.m[1][3]*m.m[2][1]);


	inv[12] = invDet*(-m.m[1][0]*m.m[2][1]*m.m[3][2]-m.m[1][1]*m.m[2][2]*m.m[3][0]-m.m[1][2]*m.m[2][0]*m.m[3][1]
					 + m.m[1][2]*m.m[2][1]*m.m[3][0]+m.m[1][1]*m.m[2][0]*m.m[3][2]+m.m[1][0]*m.m[2][2]*m.m[3][1]);
	inv[13] = invDet*( m.m[0][0]*m.m[2][1]*m.m[3][2]+m.m[0][1]*m.m[2][2]*m.m[3][0]+m.m[0][2]*m.m[2][0]*m.m[3][1]
					-  m.m[0][2]*m.m[2][1]*m.m[3][0]-m.m[0][1]*m.m[2][0]*m.m[3][2]-m.m[0][0]*m.m[2][2]*m.m[3][1]);
	inv[14] = invDet*(-m.m[0][0]*m.m[1][1]*m.m[3][2]-m.m[0][1]*m.m[1][2]*m.m[3][0]-m.m[0][2]*m.m[1][0]*m.m[3][1]
					+  m.m[0][2]*m.m[1][1]*m.m[3][0]+m.m[0][1]*m.m[1][0]*m.m[3][2]+m.m[0][0]*m.m[1][2]*m.m[3][1]);
	inv[15] = invDet*( m.m[0][0]*m.m[1][1]*m.m[2][2]+m.m[0][1]*m.m[1][2]*m.m[2][0]+m.m[0][2]*m.m[1][0]*m.m[2][1]
					-  m.m[0][2]*m.m[1][1]*m.m[2][0]-m.m[0][1]*m.m[1][0]*m.m[2][2]-m.m[0][0]*m.m[1][2]*m.m[2][1]);

	result = {
		inv[0], inv[1], inv[2], inv[4],
		inv[4], inv[5], inv[6], inv[7],
		inv[8], inv[9], inv[10],inv[11],
		inv[12],inv[13],inv[14],inv[15],
	};

	return result;
}

void Matrix4x4::Transpose()
{
	Matrix4x4 result;

	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 4; j++){
			result.m[i][j] = m[j][i];
		}
	}

	*this = result;
}

Matrix4x4 &Matrix4x4::operator*=(Matrix4x4 &m1)
{
	Matrix4x4 temp;

	for(size_t i = 0; i < 4; i++){
		for(size_t j = 0; j < 4; j++){

			for(size_t k = 0; k < 4; k++){
				temp.m[i][j] += m[i][k]*m1.m[k][j];
			}
		}
	}

	*this = temp;
	return *this;
}

bool Matrix4x4::operator==(const Matrix4x4& v) const
{
	if(
		m[0][0] == v.m[0][0] &&
		m[0][1] == v.m[0][1] &&
		m[0][2] == v.m[0][2] &&
		m[0][3] == v.m[0][3] &&

		m[1][0] == v.m[1][0] &&
		m[1][1] == v.m[1][1] &&
		m[1][2] == v.m[1][2] &&
		m[1][3] == v.m[1][3] &&

		m[2][0] == v.m[2][0] &&
		m[2][1] == v.m[2][1] &&
		m[2][2] == v.m[2][2] &&
		m[2][3] == v.m[2][3] &&

		m[3][0] == v.m[3][0] &&
		m[3][1] == v.m[3][1] &&
		m[3][2] == v.m[3][2] &&
		m[3][3] == v.m[3][3]
	){
		return true;
	}
	return false;
}



Matrix4x4 MakeIdentityMatrix()
{
	Matrix4x4 result{
		1.0f, 0.0f, 0.0f, 0.0f, 
		0.0f, 1.0f, 0.0f, 0.0f,
	    0.0f, 0.0f, 1.0f, 0.0f, 
		0.0f, 0.0f, 0.0f, 1.0f};

	return result;
}

Matrix4x4 MakeScaleMatrix(const Vector3 &scale)
{
	Matrix4x4 result;

	result = {
		scale.x, 0.0f,	  0.0f,		0.0f,
		0.0f,	 scale.y, 0.0f,		0.0f,
		0.0f,	 0.0f,	  scale.z,  0.0f,
		0.0f,	 0.0f,	  0.0f,		1.0f,
	};

	return result; 
}

Matrix4x4 MakeRotationXMatrix(float theta)
{
	float sin = std::sin(theta);
	float cos = std::cos(theta);

	Matrix4x4 result = {
		1.0f,	0.0f,		0.0f,	0.0f,
		0.0f,	cos,		sin,	0.0f,
		0.0f,	-sin,		cos,	0.0f,
		0.0f,	0.0f,		0.0f,	1.0f,
	};

	return result; 
}

Matrix4x4 MakeRotationYMatrix(float theta)
{
	float sin = std::sin(theta);
	float cos = std::cos(theta);

	Matrix4x4 result = {
		cos, 0.0f, -sin, 0.0f, 
		0.0f,1.0f, 0.0f, 0.0f,
		sin, 0.0f, cos,  0.0f, 
		0.0f,0.0f, 0.0f, 1.0f
	};

	return result;
}

Matrix4x4 MakeRotationZMatrix(float theta)
{
	float sin = std::sin(theta);
	float cos = std::cos(theta);

	Matrix4x4 result = {
		cos,  sin,  0.0f, 0.0f, 
		-sin, cos,  0.0f, 0.0f,
	    0.0f, 0.0f, 1.0f, 0.0f, 
		0.0f, 0.0f, 0.0f, 1.0f
	};

	return result;
}

Matrix4x4 MakeTranslateMatrix(const Vector3 &trans)
{
	Matrix4x4 result = {
		1.0f,	 0.0f,	  0.0f,	   0.0f,
		0.0f,	 1.0f,	  0.0f,	   0.0f,
		0.0f,	 0.0f,	  1.0f,	   0.0f,
		trans.x, trans.y, trans.z, 1.0f,
	};

	return result; 
}

Matrix4x4 MakeAffineMatrix(const Vector3 &scale, const Vector3 &rot, const Vector3 &trans)
{
	//スケーリング行列
	Matrix4x4 matScale = MakeScaleMatrix(scale);

	//回転行列
	Matrix4x4 matRotX = MakeRotationXMatrix(rot.x);
	Matrix4x4 matRotY = MakeRotationYMatrix(rot.y);
	Matrix4x4 matRotZ = MakeRotationZMatrix(rot.z);
	//合成(Z * X * Y)
	Matrix4x4 matRot = matRotZ * matRotX * matRotY;

	//平行移動行列
	Matrix4x4 matTrans = MakeTranslateMatrix(trans);

	//合成
	Matrix4x4 result = matScale*matRot*matTrans;

	return result;
}

Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip)
{
	Matrix4x4 result = {
		(1/aspectRatio)*(1/(std::tanf(fovY/2))),	0,	0,	0,
		0,	(1/(std::tanf(fovY/2))),	0,	0,
		0,	0,	farClip/(farClip-nearClip),	1,
		0,	0,	(-nearClip*farClip)/(farClip-nearClip),	0
	};

	return result;
}

Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float down, float nearClip, float farClip)
{
	Matrix4x4 result = {
		2/(right-left), 0, 0, 0,
		0, 2/(top-down), 0, 0,
		0, 0, (farClip-nearClip), 0,
		(left+right)/(left-right), (top+down)/(down-top), nearClip/(nearClip-farClip), 1		
	};

	return result;
}

Matrix4x4 operator*(Matrix4x4 &m1, Matrix4x4 &m2)
{
	Matrix4x4 result = m1;

	return result *= m2; 
}
