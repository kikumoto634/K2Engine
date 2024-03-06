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
#include "GeometryDatas/PipelineDatas.h"

//スプライト共通
class SpriteCommon
{
public:
	static SpriteCommon* GetInstance();
	static void Finalize();

	//Getter
	Pipeline* GetPipeline() const	{return datas.pipeline_.get();}
	D3D_PRIMITIVE_TOPOLOGY GetTopology() const {return datas.commandPrimitiveTopology_;}

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

	//定数
	const int RootParamsNum = 3;
	const int StaticSamplesNum = 1;
	const int InputLayoutsNum = 3;

	//Path
	const std::string VSPath = "Sprite/Sprite.VS.hlsl";
	const std::string PSPath = "Sprite/Sprite.PS.Texture.hlsl";
	
	//各要素の種類
	enum TextureDescripotrRange{
		SRV_TEXTURE_RANGE
	};
	enum StaticSamples{
		PIXEL_TEXTURE_SAMPLER
	};
	enum InputLayouts{
		POSITION,
		UV,
		NORMAL
	};
public:
	enum RootParams{
		CBV_PIXEL_MATERIAL,
		CBV_VERTEX_WVP,
		DESCRIPTOR_PIXEL_TEXTURE
	};

private:
	//パイプライン作成用構造体
	PipelineDatas datas;
};

