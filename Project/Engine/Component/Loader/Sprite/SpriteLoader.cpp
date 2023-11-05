#include "SpriteLoader.h"
#include <cassert>

#include "WindowsApp.cpp"

using namespace DirectX;
using namespace std;

ScratchImage SpriteLoader::LoadTexture(const std::string &filePath)
{
	//テクスチャファイルを読み込んでプログラムで扱えるようにする
	ScratchImage image{};
	wstring lFilePath = WindowsApp::ConvertString(filePath);

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

ID3D12Resource *SpriteLoader::CreatetextureResource(ID3D12Device *device, const DirectX::TexMetadata &metaData)
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
