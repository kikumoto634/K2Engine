#pragma once
#include <DirectXTex.h>
#include <d3d12.h>
#include <string>

#include <array>
#include <wrl.h>

#include "DirectXCommon.h"

class SpriteLoader
{
private:
	//テクスチャ最大保存枚数
	static const size_t kMaxSRVCount = 2056;

public:
	//画像読み込み
	static void LoadTexture(const std::string& filePath, DirectXCommon* dxCommon);

	//リソースGetter


	//最終的に欲しいのはテクスチャの保存先GPUHandle
	
	//Load処理、SRV設定、Heapへの場所決めは、オブジェクトの処理にはまったく関係ない。
	//というか、SRVの生成もやる必要性がわからん?　
	//StaticのLoad作成してそっちでよくね?

	//引数として、indexをもらってきて、インデックスから探すのは可能?

	//スタートハンドル + (GPUハンドルに仕様しているSize)*index
	//GetGPUDescriptorHandle作成してたのでそれを使用。DescriptorHeap.h


	//画像読み込み
	static DirectX::ScratchImage LoadTexture(const std::string& filePath, DirectX::TexMetadata& metaData);

	//テクスチャリソース作成
	static ID3D12Resource* CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metaData);

	//テクスチャリソースにデータを転送
	static void UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);

private:
	static std::string basePath;

	//改造用
	static std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, kMaxSRVCount> resources_;
	static uint32_t index_;

	static D3D12_CPU_DESCRIPTOR_HANDLE SrvHandleCPU_;	//画像のテクスチャハンドルCPU
	static D3D12_GPU_DESCRIPTOR_HANDLE SrvHandleGPU_;	//画像テクスチャハンドルGPU
};

