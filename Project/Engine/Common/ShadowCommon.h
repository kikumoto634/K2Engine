#pragma once
#include "DirectXCommon.h"

class ShadowCommon
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	void Initialize();

	void PreDraw();
	void PostDraw();

private:
	DirectXCommon* dxCommon = nullptr;

	//シャドウマップ用の深度バッファ用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> shadowDescriptorHeap_;
	//シャドウマップ用の深度バッファ
	ComPtr<ID3D12Resource> shadowResources_;
};

