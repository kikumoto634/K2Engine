#pragma once

//全体でCommon内で生成したものを使用
//新規で制作したい場合、基底クラスにPipelineなど必要なものを持たせておく。
//その中身、がNUllの時個別処理

//共有にまとめるもの(2023/02/02)現在基底クラスにあるもので
//pipeline
//rootParamter
// inputLayout
// staticSamplers
// shaderPath
// 描画形状
// カリング
//

#include <d3d12.h>
#include <memory>
#include <vector>
#include "Pipeline.h"

//スプライト共通
class SpriteCommon
{
private:
	template <class T> using unique_ptr = std::unique_ptr<T>;
	template <class T> using vector = std::vector<T>;

public:
	static SpriteCommon* GetInstance();
	static void Finalize();

	//Getter
	Pipeline* GetPipeline() const	{return pipeline_.get();}
	D3D_PRIMITIVE_TOPOLOGY GetTopology() const {return commandPrimitiveTopology_;}

private:
	SpriteCommon(){};
	~SpriteCommon() = default;

	//初期化
	void Initialize();

public:
	SpriteCommon(const SpriteCommon& obj) = delete;
	SpriteCommon operator=(const SpriteCommon& obj) = delete;


private:
	static SpriteCommon* instance_;

private:
	unique_ptr<Pipeline> pipeline_;
	vector<D3D12_ROOT_PARAMETER> rootParameters_;			//ルートパラメータ
	vector<D3D12_INPUT_ELEMENT_DESC> inputElementDesc_;		//インプットレイアウト
	vector<D3D12_STATIC_SAMPLER_DESC> staticSamplers_;		//サンプラー

	D3D12_PRIMITIVE_TOPOLOGY_TYPE pipelinePrimitiveTopology_ = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;	//パイプライン
	D3D_PRIMITIVE_TOPOLOGY commandPrimitiveTopology_ = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;				//コマンドリスト

	D3D12_FILL_MODE fillMode_ = D3D12_FILL_MODE_SOLID;	//塗りつぶし

	std::string VSPath_ = "Sprite/Sprite.VS.hlsl";
	std::string PSPath_ = "Sprite/Sprite.PS.Texture.hlsl";
};

