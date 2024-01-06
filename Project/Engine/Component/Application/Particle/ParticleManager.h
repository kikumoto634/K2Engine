#pragma once
#include <d3d12.h>

//パーティクルのSRVの生成が複数回行われているのが問題
/*
* まとめるべきもの
* ・SRV生成
* ・その為に、最大生成数、SRVに送る構造体・リソース
*/

class ParticleManager
{
private:
	//最大出現数
	const int kNumMaxInstance = 100;
	//時間
	const float kDeltaTime = 1.0f/60.f;


	D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU_;
	D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU_;
};

