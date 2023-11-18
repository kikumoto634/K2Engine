#pragma once
#include <DirectXTex.h>
#include <array>
#include <wrl.h>

#include "DirectXCommon.h"
#include "Texture.h"

class SpriteLoader
{

private:
	//テクスチャ最大保存枚数
	static const size_t kMaxSRVCount = 2056;

public:
	//画像読み込み
	static Texture LoadTexture(const std::string& filePath, DirectXCommon* dxCommon);
	static void SetTextureSRVDescriptor(UINT rootParamterIndex, Texture tex, DirectXCommon* dxCommon);

	//リソースGetter


	//最終的に欲しいのはテクスチャの保存先GPUHandle
	
	//Load処理、SRV設定、Heapへの場所決めは、オブジェクトの処理にはまったく関係ない。
	//というか、SRVの生成もやる必要性がわからん?　
	//StaticのLoad作成してそっちでよくね?

	//引数として、indexをもらってきて、インデックスから探すのは可能?

	//スタートハンドル + (GPUハンドルに仕様しているSize)*index
	//GetGPUDescriptorHandle作成してたのでそれを使用。DescriptorHeap.h


	//Textureに情報を保存
	//フォルダないの画像を自動読み
	//ファイル名で呼び出せるようにする

	//そうなると、呼び出す際インデックスが指定できない分処理がかさむ
	//Baseに必要となるindexの保存変数を持たせる

	//1.ビルド後に自動ロード
	//2.オブジェクトたちは、画像名から必要なインデックスをもらう。
	//3.それと同時にオブジェクト(Base)が保持しているインデックス格納変数に代入
	//4.インデックスを基に、Loader側でcommandListとインデックスを渡して必要画像のハンドルをSRVに受け渡す


private:
	//画像読み込み
	static DirectX::ScratchImage Load(const std::string& filePath, DirectX::TexMetadata& metaData);

	//テクスチャリソース作成
	static ID3D12Resource* CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metaData);

	//テクスチャリソースにデータを転送
	static void UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);

private:
	static std::string basePath;

	//改造用
	//static std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, kMaxSRVCount> resources_;
	static uint32_t index_;
};

