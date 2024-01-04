#pragma once
#include <wrl.h>
#include <dxcapi.h>
#include <vector>

#include "Transform.h"
#include "Pipeline.h"

#include "Texture.h"
#include "VertexData.h"
#include "MaterialData.h"
#include "TransformationMatrixData.h"

class ParticleBase
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	template <class T> using vector = std::vector<T>;
public:
	~ParticleBase(){
		delete pipeline_;
	}
	virtual void Draw(Matrix4x4 viewProjectionMatrix);

protected:
	//初期化
	virtual void Initialize(bool isIndexEnable = true);


	virtual void ApplyGlobalVariablesInitialize();
	virtual void ApplyGlobalVariablesUpdate();

private:
	//パイプライン
	void PipelineStateInitialize();

protected:
	//頂点リソース/ビュー
	virtual void CreateVertex();
	//インデックスリソース/ビュー
	virtual void CreateIndex();
	//定数リソース/ビュー
	virtual void CreateMaterial();
	//行列リソース/ビュー
	virtual void CreateWVP();

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

	ComPtr<ID3D12Resource> constResource_;		//定数
	GeometryMaterial* materialData_ = nullptr;

	ComPtr<ID3D12Resource> wvpResource_;		//行列
	TransformationMatrix* wvpData_ = nullptr;

	//描画方法
	D3D12_PRIMITIVE_TOPOLOGY_TYPE pipelinePrimitiveTopology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;	//パイプライン
	D3D_PRIMITIVE_TOPOLOGY commandPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;				//コマンドリスト

protected:
	const char* name = "";

	//頂点データ
	VertexData* vertData_ = nullptr;
	UINT vertNum_ = 4;

	//インデックスデータ
	bool isIndexDataEnable_ = true;
	uint32_t* indexData_ = nullptr;
	UINT indexNum_ = 4;

	D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU;
	D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU;

	D3D12_FILL_MODE fillMode = D3D12_FILL_MODE_SOLID;	//塗りつぶし

	BlendSetting::BlendMode blendMode = BlendSetting::BlendMode::kBlendModeNormal;

	//パラメータ
	int kNumInstance = 10;
	vector<Transform> transforms;
	Transform baseTransform = {{0,0,0},{0,0,0},{1,1,1}};

	Vector4 color_ = {1.0f, 1.0f, 1.0f, 1.0f};

	std::string texturePath_ = "uvChecker.png";

	std::string VSPath_ = "Particle/Particle.VS.hlsl";
	std::string PSPath_ = "Particle/Particle.PS.hlsl";
};

