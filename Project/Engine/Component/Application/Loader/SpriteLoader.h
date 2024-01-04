#pragma once
#include <DirectXTex.h>
#include <array>
#include <wrl.h>

#include "DirectXCommon.h"
#include "Texture.h"

class SpriteLoader
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	template <class T> using vector = std::vector<T>;

public:
	//テクスチャ最大保存枚数
	static const size_t kMaxSRVCount = 2056;
	//SRV保存先Offset
	static const int SRVOffset = 2;

public:
	//フォルダ名から必要なテクスチャデータを確保
	static Texture SearchTexture(std::string textureName);
	//画像読み込み
	static void LoadTexture(DirectXCommon* dxCommon);
	static void LoadTexture(DirectXCommon* dxCommon, std::string filePath);

	static std::array<Texture, kMaxSRVCount> GetTexture()	{return textures_;}

private:
	//フォルダ内の画像名全取得
	static std::vector<std::string> getImageName();

	//画像読み込み
	static DirectX::ScratchImage Load(const std::string& filePath, DirectX::TexMetadata& metaData);

	//テクスチャリソース作成
	static ID3D12Resource* CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metaData);

	//テクスチャリソースにデータを転送
	static void UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);

private:
	static std::string basePath_;
	//フォルダ内の画像全て
	static vector<std::string> files_;

	//テクスチャリソース
	static std::array<ComPtr<ID3D12Resource>, kMaxSRVCount> resources_;
	static uint32_t index_;

	static std::array<Texture, kMaxSRVCount> textures_;
};

