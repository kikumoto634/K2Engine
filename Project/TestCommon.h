#pragma once

#include "GeometryDatas/PipelineDatas.h"

class DirectXCommon;
class TestCommon
{
public:
	static TestCommon* GetInstance();
	static void Finalize();

	void Draw();

	//Getter
	Pipeline* GetPipeline() const	{return datas.pipeline_.get();}
	D3D_PRIMITIVE_TOPOLOGY GetTopology() const {return datas.commandPrimitiveTopology_;}

private:
	TestCommon(){};
	~TestCommon() = default;

	//初期化
	void Initialize();

public:
	TestCommon(const TestCommon& obj) = delete;
	TestCommon operator=(const TestCommon& obj) = delete;
	
private:
	static TestCommon* instance_;

	//定数
	const int RootParamsNum = 5;
	const int StaticSamplesNum = 1;
	const int InputLayoutsNum = 3;

	//Path
	const std::string VSPath = "TestVS.hlsl";
	const std::string PSPath = "TestPS.hlsl";
	
	//各要素の種類
	enum TextureDescripotrRange{
		SRV_TEXTURE_RANGE
	};
	//行列
	enum WVPDescripotrRange{
		SRV_WVP_RANGE
	};
	//マテリアル
	enum MaterialDescriptorRange{
		SRV_MATERIAL_RANGE
	};
	//カメラ
	enum CameraDescriptorRange{
		SRV_CAMERA_RANGE
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
		DESCRIPTOR_VERTEX_WVP,
		DESCRIPTOR_PIXEL_MATERIAL,
		DESCRIPTOR_PIXEL_CAMERA
	};

private:
	DirectXCommon* dxCommon = nullptr;

	//パイプライン作成用構造体
	PipelineDatas datas;
};

