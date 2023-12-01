#pragma once
#include "Common/GeometryBase.h"
class Sphere : public GeometryBase
{
public:
	static Sphere* Create(Transform transform = {{0,0,0}, {0,0,0}, {1,1,1}});

public:
	Sphere(Transform transform){
		this->transform.translate = transform.translate;
		this->transform.rotation = transform.rotation;
		this->transform.scale = transform.scale;

		vertNum_ = kSubdivision*kSubdivision*4;
		indexNum_ = kSubdivision*kSubdivision*6;

		texturePath_ = "monsterBall.png";
	}
	~Sphere(){
		delete pipeline_;
	}

	void Update();

	void Draw(Matrix4x4 viewProjectionMatrix) override;

private:
	void PipelineChange();

	void SphereVertexData();
	void SphereIndexData();

private:
	//球体分割数
	const uint32_t kSubdivision = 16;

	//個別パイプライン
	std::vector<D3D12_ROOT_PARAMETER> rootParameters_;			//ルートパラメータ
	std::vector<D3D12_STATIC_SAMPLER_DESC> staticSamplers_;		//サンプラー
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDesc_;		//インプットレイアウト
	D3D12_PRIMITIVE_TOPOLOGY_TYPE pipelinePrimitiveTopology;	//パイプライン
	D3D_PRIMITIVE_TOPOLOGY commandPrimitiveTopology;				//コマンドリスト
	D3D12_FILL_MODE fillMode;	//塗りつぶし
	std::string VSPath_;
	std::string PSPath_;

	//パイプライン関係
	Pipeline* pipeline_ = nullptr;
};

