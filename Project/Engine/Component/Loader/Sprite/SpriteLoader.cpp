#include "SpriteLoader.h"
#include <cassert>
#include <WindowsApp.h>

using namespace DirectX;
using namespace std;

string SpriteLoader::basePath = "Resources/";

ScratchImage SpriteLoader::LoadTexture(const std::string &filePath)
{
	const string fullPath = basePath + filePath;

	//テクスチャファイルを読み込んでプログラムで扱えるようにする
	ScratchImage image{};
	wstring lFilePath = WindowsApp::ConvertString(fullPath);

	HRESULT result = LoadFromWICFile(lFilePath.c_str(), WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(result));

	//ミップマップ
	ScratchImage mipImages{};
	result = GenerateMipMaps(
		image.GetImages(), image.GetImageCount(), image.GetMetadata(),
		TEX_FILTER_SRGB, 0, mipImages
	);
	assert(SUCCEEDED(result));

	return image;
}


ID3D12Resource *SpriteLoader::CreateTextureResource(ID3D12Device *device, const DirectX::TexMetadata &metaData)
{
	//1.metaDataを元にResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metaData.width);	//画像幅
	resourceDesc.Height = UINT(metaData.height);	//画面高さ
	resourceDesc.MipLevels = UINT16(metaData.mipLevels);	//mipMap数
	resourceDesc.DepthOrArraySize = UINT16(metaData.arraySize);	//奥行 or 配列Textureの要素数
	resourceDesc.Format = metaData.format;	//TextureのFormat
	resourceDesc.SampleDesc.Count = 1;	//サンプリングカウント
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metaData.dimension);	//Textureの次元数

	//2.利用するHeap設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;	//WriteBackポリシーでCPUアクセス可能
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;	//プロセッサの近く

	//3.Rsource作成
	ID3D12Resource* resource = nullptr;
	HRESULT result = device->CreateCommittedResource(
		&heapProperties,					//Heap
		D3D12_HEAP_FLAG_NONE,				//Heap特殊設定
		&resourceDesc,						//Resource
		D3D12_RESOURCE_STATE_GENERIC_READ,	//ResourceState TextureはReadOnly
		nullptr,							//Clear最適値
		IID_PPV_ARGS(&resource)				//作成するResourceポインタへのポインタ
	);
	assert(SUCCEEDED(result));

	return resource;
}

ID3D12Resource *SpriteLoader::CreateDepthStencilTextureResource(ID3D12Device *device, int32_t width, int32_t height)
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


void SpriteLoader::UploadTextureData(ID3D12Resource *texture, const DirectX::ScratchImage &mipImages)
{
	//Meta情報を取得
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	//全MipMapについて
	for(size_t mipLevel = 0; mipLevel < metadata.mipLevels; ++mipLevel){
		//MipMapLevelを指定してImage取得
		const DirectX::Image* img = mipImages.GetImage(mipLevel, 0, 0);
		//転送(アドレス経由)
		HRESULT result = texture->WriteToSubresource(
			UINT(mipLevel),
			nullptr,				//全領域へコピー
			img->pixels,			//元のデータアドレス
			UINT(img->rowPitch),	//1ラインサイズ
			UINT(img->slicePitch)	//1毎サイズ
		);
		assert(SUCCEEDED(result));
	}
}
