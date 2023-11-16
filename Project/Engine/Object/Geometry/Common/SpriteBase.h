#pragma once
#include <wrl.h>
#include <dxcapi.h>
#include <vector>

#include "Pipeline.h"
#include "DirectXCommon.h"
#include "Transform.h"

#include "VertexData.h"
#include "MaterialData.h"
#include "TransformationMatrixData.h"
#include "DirectionalLightData.h"

class SpriteBase : public Transform
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	template <class T> using vector = std::vector<T>;

public:
	~SpriteBase(){
		delete pipeline_;
	}
	void Draw(Matrix4x4 viewProjectionMatrix);

protected:
	//初期化
	void Initialize(bool isIndexEnable = true);

private:

	//テクスチャSRV
	void TextureSRVInitialize();
	//パイプライン
	void PipelineStateInitialize();

	//頂点リソース/ビュー
	void CreateVertex();
	//インデックスリソース/ビュー
	void CreateIndex();
	//定数リソース/ビュー
	void CreateMaterial();
	//行列リソース/ビュー
	void CreateWVP();

private:
	//Instance
	DirectXCommon* dxCommon = nullptr;

	//パイプライン関係
	Pipeline* pipeline_ = nullptr;
	vector<D3D12_ROOT_PARAMETER> rootParameters_;			//ルートパラメータ
	vector<D3D12_INPUT_ELEMENT_DESC> inputElementDesc_;		//インプットレイアウト
	vector<D3D12_STATIC_SAMPLER_DESC> staticSamplers_;		//サンプラー


	//リソース関係
	ComPtr<ID3D12Resource> vertexResource_;		//頂点
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;

	ComPtr<ID3D12Resource> indexResource_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_;

	ComPtr<ID3D12Resource> constResource_;		//定数
	TextureMaterial* materialData_ = nullptr;

	ComPtr<ID3D12Resource> wvpResource_;		//行列
	TransformationMatrix* wvpData_ = nullptr;


	//テクスチャ関係
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU_ = {};	//画像のテクスチャハンドルCPU
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_ = {};	//画像テクスチャハンドルGPU

	//描画方法
	D3D12_PRIMITIVE_TOPOLOGY_TYPE pipelinePrimitiveTopology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;	//パイプライン
	D3D_PRIMITIVE_TOPOLOGY commandPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;				//コマンドリスト


protected:
	Transform uvTransformSprite{
		{0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 0.0f},
		{1.0f, 1.0f, 1.0f},
	};

	//頂点データ
	VertexData* vertData_ = nullptr;
	UINT vertNum_ = 4;

	//インデックスデータ
	uint32_t* indexData_ = nullptr;
	UINT indexNum_ = 6;

	D3D12_FILL_MODE fillMode = D3D12_FILL_MODE_SOLID;	//塗りつぶし

	//パラメータ
	Vector4 color_ = {1.0f, 1.0f, 1.0f, 1.0f};

	std::string texturePath_ = "uvChecker.png";

	std::string VSPath_ = "Sprite/Sprite.VS.hlsl";
	std::string PSPath_ = "Sprite/Sprite.PS.Texture.hlsl";
};

