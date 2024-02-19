#include "FrustumCulling.h"
#include "MathUtility.h"

void FrustumCulling::Initialize()
{
	planes.resize(6);
	points.resize(8);
}

void FrustumCulling::Update(Camera *camera)
{
	//面
	Matrix4x4 vp = camera->GetViewProjectionMatrix();
	vp.Transpose();

	Vector4 vRight = {vp.m[0][0],vp.m[0][1],vp.m[0][2],vp.m[0][3]};
	Vector4 vUp = {vp.m[1][0],vp.m[1][1],vp.m[1][2],vp.m[1][3]};
	Vector4 vForward = {vp.m[2][0],vp.m[2][1],vp.m[2][2],vp.m[2][3]};
	Vector4 vPos = {vp.m[3][0],vp.m[3][1],vp.m[3][2],vp.m[3][3]};

	planes[0] = vPos + vRight;		//left
	planes[1] = vPos - vRight;		//right
	planes[2] = vPos + vUp;			//bottom
	planes[3] = vPos - vUp;			//top
	planes[4] = vForward;			//near
	planes[5] = vPos - vForward;	//far


	//頂点
	Vector4 corners[8];
	corners[0] = {-1,-1,-1,+1};
	corners[1] = {+1,-1,-1,+1};
	corners[2] = {+1,+1,-1,+1};
	corners[3] = {-1,+1,-1,+1};
	corners[4] = {-1,-1,+1,+1};
	corners[5] = {+1,-1,+1,+1};
	corners[6] = {+1,+1,+1,+1};
	corners[7] = {-1,+1,+1,+1};
	Matrix4x4 invVp = invVp.Inverse(camera->GetViewProjectionMatrix());

	for(int i = 0; i != 8; i++){
		Vector4 q = Multiplication(corners[i], invVp);
		points[i] = q / q.w;
	}
}
