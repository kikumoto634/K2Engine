#pragma once
#include <wrl.h>

#include "DirectXCommon.h"

#include "DescriptorData.h"
//シャドウマップ専用のヒープ作成
//シャドウマップ用のステンシルバッファ

//ステンシルバッファを使用し、光源の視点から深度情報を取得　これをシャドウマップという
//通常テクスチャとして扱う

//シャドウマップを使用してシャドウエフェクトの計算

class ShadowMapCommon
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	static ShadowMapCommon* GetInstance();

public:
	void Initialize();

	ID3D12Resource* GetShadowResource()	{return shadowResource_.Get();}

private:
	static ShadowMapCommon* instance_;

public:
	DirectXCommon* dxCommon = nullptr;

	//深度ヒープ
	DescriptorDSVData shadowDSVDescriptorHeap_;
	uint32_t shadowDSVDescriptorHeapSize_;

	//深度テクスチャヒープ
	DescriptorSRVData shadowTextureSRVDescriptorHeap_;
	uint32_t shadowTextureSRVDescriptorHeapSize_;

	//深度バッファ
	ComPtr<ID3D12Resource> shadowResource_;
};

