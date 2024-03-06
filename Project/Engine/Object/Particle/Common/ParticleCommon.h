#pragma once
#include "GeometryDatas/PipelineDatas.h"

class ParticleCommon
{
public:
	static ParticleCommon* GetInstance();
	static void Finalize();
	
	//Getter
	Pipeline* GetPipeline() const	{return datas.pipeline_.get();}
	D3D_PRIMITIVE_TOPOLOGY GetTopology() const {return datas.commandPrimitiveTopology_;}

private:
	ParticleCommon(){};
	~ParticleCommon() = default;

	void Initialize();

public:
	ParticleCommon(const ParticleCommon& obj)	= delete;
	ParticleCommon operator=(const ParticleCommon& obj) = delete;

private:
	static ParticleCommon* instance_;

	//定数
	const int RootParamsNum = 2;
	const int StaticSamplesNum = 1;
	const int InputLayoutsNum = 2;

	//Path
	std::string VSPath_ = "Particle/Particle.VS.hlsl";
	std::string PSPath_ = "Particle/Particle.PS.hlsl";
	
	//各要素の種類
	enum TextureDescripotrRange{
		SRV_TEXTURE_RANGE
	};
	enum WVPDescripotrRange{
		SRV_WVP_RANGE
	};
	enum StaticSamples{
		PIXEL_TEXTURE_SAMPLER
	};
	enum InputLayouts{
		POSITION,
		TEXCOORD
	};
public:
	enum RootParams{
		DESCRIPTOR_PIXEL_TEXTURE,
		DESCRIPTOR_VERTEX_WVP
	};

private:
	//パイプライン作成用構造体
	PipelineDatas datas;
};

