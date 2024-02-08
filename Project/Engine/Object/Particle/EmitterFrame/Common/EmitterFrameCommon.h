#pragma once
#include "GeometryDatas/PipelineDatas.h"

class EmitterFrameCommon
{
public:
	static EmitterFrameCommon* GetInstance();
	static void Finalize();
	
	//Getter
	Pipeline* GetPipeline() const	{return datas.pipeline_.get();}
	D3D_PRIMITIVE_TOPOLOGY GetTopology() const {return datas.commandPrimitiveTopology_;}

private:
	EmitterFrameCommon(){};
	~EmitterFrameCommon() = default;

	void Initialize();

public:
	EmitterFrameCommon(const EmitterFrameCommon& obj)	= delete;
	EmitterFrameCommon operator=(const EmitterFrameCommon& obj) = delete;

private:
	static EmitterFrameCommon* instance_;

	//定数
	const int RootParamsNum = 2;
	const int InputLayoutsNum = 1;

	//Path
	std::string VSPath_ = "Particle/Emitter/EmitterFrame.VS.hlsl";
	std::string PSPath_ = "Particle/Emitter/EmitterFrame.PS.hlsl";
	
	//各要素の種類
	enum InputLayouts{
		POSITION
	};
public:
	enum RootParams{
		CBV_VERTEX_WVP,
		CBV_PIXEL_Material
	};

private:
	//パイプライン作成用構造体
	PipelineDatas datas;
};

