#pragma once
#include "GeometryDatas/PipelineDatas.h"

class GPUParticleCommon
{
public:
	static GPUParticleCommon* GetInstance();
	static void Finalize();
	
	//Getter
	Pipeline* GetPipeline() const	{return datas.pipeline_.get();}
	D3D_PRIMITIVE_TOPOLOGY GetTopology() const {return datas.commandPrimitiveTopology_;}

private:
	GPUParticleCommon(){};
	~GPUParticleCommon() = default;

	void Initialize();

public:
	GPUParticleCommon(const GPUParticleCommon& obj)	= delete;
	GPUParticleCommon operator=(const GPUParticleCommon& obj) = delete;

private:
	static GPUParticleCommon* instance_;

	//定数
	const int RootParamsNum = 2;
	const int InputLayoutsNum = 1;

	//Path
	std::string VSPath_ = "Particle/GPUParticle.VS.hlsl";
	std::string PSPath_ = "Particle/Emitter/EmitterFrame.PS.hlsl";
	
	//各要素の種類
	enum WVPDescripotrRange{
		SRV_WVP_RANGE
	};
	enum InputLayouts{
		POSITION
	};
public:
	enum RootParams{
		DESCRIPTOR_VERTEX_WVP,
		CBV_PIXEL_MATERIAL
	};

private:
	//パイプライン作成用構造体
	PipelineDatas datas;
};

