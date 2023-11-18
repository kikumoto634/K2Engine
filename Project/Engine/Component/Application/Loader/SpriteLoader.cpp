#include "SpriteLoader.h"
#include <cassert>
#include <WindowsApp.h>

#include "DescriptorHeap.h"

using namespace DirectX;
using namespace std;

string SpriteLoader::basePath = "Resources/";
uint32_t SpriteLoader::index_ = 0;

//Texture SpriteLoader::LoadTexture(const std::string &filePath, DirectXCommon* dxCommon)
//{
//	if(index_ >= kMaxSRVCount){
//		WindowsApp::Log("テクスチャ保存数が最大を超えました");
//		assert(0);
//	}
//
//	Texture tex;
//	DirectX::TexMetadata metaData;
//	DirectX::ScratchImage mipImages;
//	
//	const string fullPath = basePath + filePath;
//
//	mipImages = Load(fullPath, metaData);
//	resources_[index_] = CreateTextureResource(dxCommon->GetDevice(), metaData);
//	UploadTextureData(resources_[index_].Get() ,mipImages);
//
//	//SRV設定
//	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
//	srvDesc.Format = metaData.format;
//	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
//	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
//	srvDesc.Texture2D.MipLevels = UINT(metaData.mipLevels);
//
//	//Heapのハンドル
//	tex.srvHandleCPU_ = GetCPUDescriptorHandle(dxCommon->GetSRVDescriptorHeap(), dxCommon->GetDescriptorSizeSRV(), index_ + 1);
//	tex.srvHandleGPU_ = GetGPUDescriptorHandle(dxCommon->GetSRVDescriptorHeap(), dxCommon->GetDescriptorSizeSRV(), index_ + 1);
//
//	//SRV生成
//	dxCommon->GetDevice()->CreateShaderResourceView(resources_[index_].Get(), &srvDesc, tex.srvHandleCPU_);
//
//	tex.filePath = fullPath;
//	tex.index = index_;
//
//	index_++;
//	return tex;
//}

void SpriteLoader::SetTextureSRVDescriptor(UINT rootParamterIndex, Texture tex, DirectXCommon* dxCommon)
{
	dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(rootParamterIndex, tex.srvHandleGPU_);
}



ScratchImage SpriteLoader::Load(const std::string &filePath, DirectX::TexMetadata& metaData)
{
	//テクスチャファイルを読み込んでプログラムで扱えるようにする
	ScratchImage image{};
	wstring lFilePath = WindowsApp::ConvertString(filePath);

	HRESULT result = LoadFromWICFile(lFilePath.c_str(), WIC_FLAGS_FORCE_SRGB, &metaData, image);
	assert(SUCCEEDED(result));

	//ミップマップ
	ScratchImage mipImages{};
	result = GenerateMipMaps(
		image.GetImages(), image.GetImageCount(), image.GetMetadata(),
		TEX_FILTER_SRGB, 0, mipImages
	);
	if(SUCCEEDED(result)){
		image = std::move(mipImages);
		metaData = image.GetMetadata();
	}

	metaData.format = MakeSRGB(metaData.format);

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
