#pragma once
#include <DirectXTex.h>
#include <d3d12.h>
#include <string>

#include <array>
#include <wrl.h>

class SpriteLoader
{
private:
	//テクスチャ最大保存枚数
	static const size_t kMaxSRVTextureCount = 2056;

public:
	//画像読み込み
	static uint32_t LoadTexture(const std::string& filePath, ID3D12Device* device);

	//リソースGetter




	//画像読み込み
	static DirectX::ScratchImage LoadTexture(const std::string& filePath, DirectX::TexMetadata& metaData);

	//テクスチャリソース作成
	static ID3D12Resource* CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metaData);

	//テクスチャリソースにデータを転送
	static void UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);

private:
	static std::string basePath;

	//改造用
	static std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, kMaxSRVTextureCount> textureResources_;
	static uint32_t textureIndex;
};

