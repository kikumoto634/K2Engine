#pragma once

#include "GeometryDatas/PipelineDatas.h"

class DirectXCommon;
class GeometryCommon
{
public:
	static GeometryCommon* GetInstance();
	static void Finalize();

	void Draw();

	//Getter
	Pipeline* GetPipeline() const	{return datas.pipeline_.get();}
	D3D_PRIMITIVE_TOPOLOGY GetTopology() const {return datas.commandPrimitiveTopology_;}

private:
	GeometryCommon(){};
	~GeometryCommon() = default;

	//初期化
	void Initialize();

public:
	GeometryCommon(const GeometryCommon& obj) = delete;
	GeometryCommon operator=(const GeometryCommon& obj) = delete;
	
private:
	static GeometryCommon* instance_;

	//定数
	const int RootParamsNum = 5;
	const int StaticSamplesNum = 1;
	const int InputLayoutsNum = 3;

	//Path
	const std::string VSPath = "Object3D/Object3D.VS.hlsl";
	const std::string PSPath = "Object3D/Object3D.PS.Texture.hlsl";
	
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
		DESCRIPTOR_PIXEL_TEXTURE,
		CBV_ALL_LIGHT,
		CBV_VERTEX_WVP,
		CBV_PIXEL_MATERIAL,
		CBV_PIXEL_CAMERA
	};

private:
	DirectXCommon* dxCommon = nullptr;

	//パイプライン作成用構造体
	PipelineDatas datas;
};

