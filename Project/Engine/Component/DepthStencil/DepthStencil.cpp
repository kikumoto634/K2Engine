#include "DepthStencil.h"
#include <cassert>

ID3D12Resource* CreateDepthStencilTextureResource(ID3D12Device *device, int32_t width, int32_t height)
{
	//Resourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;										//テクスチャの幅
	resourceDesc.Height = height;									//高さ
	resourceDesc.MipLevels = 1;										//mipMap数
	resourceDesc.DepthOrArraySize = 1;								//奥行・配列Textureの配列数
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;			//DepthStencilとして利用可能のフォーマット
	resourceDesc.SampleDesc.Count = 1;								//1固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;	//2次元
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;	//DepthStencilとして使用

	//Heap設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;	//VRAM上に作成

	//深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;	//1.0(最大値)でクリア
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	//フォーマット。Resourceに合わせる

	//Resource作成
	ID3D12Resource* resource = nullptr;
	HRESULT result = device->CreateCommittedResource(
		&heapProperties,					//Heap設定
		D3D12_HEAP_FLAG_NONE,				//Heapの特殊設定はなし
		&resourceDesc,						//Resource設定
		D3D12_RESOURCE_STATE_DEPTH_WRITE,	//深度値を書き込む状態にする
		&depthClearValue,					//Clear最適値
		IID_PPV_ARGS(&resource)				//生成するResourceポインタ
	);
	assert(SUCCEEDED(result));

	return resource;
}