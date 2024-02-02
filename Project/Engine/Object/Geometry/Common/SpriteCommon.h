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

private:
	PipelineDatas datas;
};

