#pragma once
#include <wrl.h>
#include <dxcapi.h>
#include <vector>
#include <list>

#include "Transform.h"
#include "Pipeline.h"

#include "Camera.h"

#include "Texture.h"
#include "VertexData.h"
#include "MaterialData.h"
#include "ParticleData.h"
#include "ParticleForGPUData.h"

#include <random>

class ParticleBase
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	template <class T> using vector = std::vector<T>;
	template <class T> using list = std::list<T>;
public:
	~ParticleBase(){
		delete pipeline_;
	}
	virtual void Update();
	virtual void Draw(Camera* camera);

	virtual void Add(const Vector3& translate = {0,0,0});
protected:
	//初期化
	virtual void Initialize(bool isIndexEnable = true);

	virtual ParticleData MakeNewParticle(std::mt19937 &randomEngine, const Vector3& translate);

	virtual void ApplyGlobalVariablesInitialize();
	virtual void ApplyGlobalVariablesUpdate();

private:
	//パイプライン
	void PipelineStateInitialize();

protected:
	//頂点リソース/ビュー
	void CreateVertex();
	//インデックスリソース/ビュー
	void CreateIndex();
	//行列リソース/ビュー
	void CreateWVP();

protected:
	//Instance
	DirectXCommon* dxCommon = nullptr;

	//パイプライン関係
	Pipeline* pipeline_ = nullptr;

	vector<D3D12_ROOT_PARAMETER> rootParameters_;			//ルートパラメータ
	vector<D3D12_INPUT_ELEMENT_DESC> inputElementDesc_;		//インプットレイアウト
	vector<D3D12_STATIC_SAMPLER_DESC> staticSamplers_;		//サンプラー

	//テクスチャ情報
	Texture texture_;

	//リソース関係
	ComPtr<ID3D12Resource> vertexResource_;		//頂点
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;

	ComPtr<ID3D12Resource> indexResource_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_;

	ComPtr<ID3D12Resource> particleResource_;		//行列
	ParticleForGPUData* particleData_ = nullptr;
	D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU_;
	D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU_;

	//描画方法
	D3D12_PRIMITIVE_TOPOLOGY_TYPE pipelinePrimitiveTopology_ = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;	//パイプライン
	D3D_PRIMITIVE_TOPOLOGY commandPrimitiveTopology_ = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;				//コマンドリスト

protected:
	const char* name_ = "";

	//頂点データ
	VertexData* vertData_ = nullptr;
	UINT vertNum_ = 4;

	//インデックスデータ
	bool isIndexDataEnable_ = true;
	uint32_t* indexData_ = nullptr;
	UINT indexNum_ = 4;

	//ビルボード用
	const char* billboardTypeName[3] = { "OFF", "ALL_AXIS", "Y_AXIS"};
    bool billboardTypeEnable[3] = { true, false, false}; //
	Matrix4x4 billboardMatrix_;

	D3D12_FILL_MODE fillMode_ = D3D12_FILL_MODE_SOLID;	//塗りつぶし

	BlendSetting::BlendMode blendMode_ = BlendSetting::BlendMode::kBlendModeAdd;

	//パラメータ
	//最大数
	int kNumMaxInstance_ = 100;
	//パーティクル変数
	list<ParticleData> particles_;

	const float kDeltaTime_ = 1.0f/60.f;

	std::string texturePath_ = "uvChecker.png";

	std::string VSPath_ = "Particle/Particle.VS.hlsl";
	std::string PSPath_ = "Particle/Particle.PS.hlsl";
};

