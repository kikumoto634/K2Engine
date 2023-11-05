#pragma once
#include <DirectXTex.h>
#include <d3d12.h>
#include <string>

class SpriteLoader
{
public:
	//画像読み込み
	static DirectX::ScratchImage LoadTexture(const std::string& filePath);

	//テクスチャリソース作成
	static ID3D12Resource* CreatetextureResource(ID3D12Device* device, const DirectX::TexMetadata& metaData);
};

