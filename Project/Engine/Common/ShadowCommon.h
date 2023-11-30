#pragma once
#include "DirectXCommon.h"

#include "ShadowPipeline.h"

class ShadowCommon
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	~ShadowCommon(){
		delete pipeline_;
	}

	void Initialize();

	void PreDraw();
	void PostDraw();

private:
	DirectXCommon* dxCommon = nullptr;

	//シャドウマップ用の深度バッファ
	ComPtr<ID3D12Resource> shadowResources_;
	//シャドウマップ用の深度バッファビュー
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	//シャドウマップ用のDSVヒープ
	ComPtr<ID3D12DescriptorHeap> shadowDSVHeap{};

	ShadowPipeline* pipeline_ = nullptr;
};

