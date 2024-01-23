#pragma once
#include <wrl.h>
#include <dxcapi.h>
#include <vector>

#include "Pipeline.h"

#include "DirectXCommon.h"
#include "Transform.h"
#include "Texture.h"

#include "VertexData.h"
#include "MaterialData.h"
#include "TransformationMatrixData.h"

#include "Camera.h"

#include "Geometry/Common/BaseCollider.h"

#include "ShadowMapCommon.h"

//幾何学オブジェクトの共通
class GeometryBase : public Transform
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	template <class T> using vector = std::vector<T>;
public:
	~GeometryBase();
	virtual void Update();
	virtual void Draw(Camera* camera);

	virtual void OnCollision(const CollisionInfo& info) {}

	//Getter/Setter
	Transform GetTransform() const {return {translate,rotation,scale};}
	BaseCollider* GetCollider()	{return collider_;}

protected:
	//初期化
	void Initialize(bool isIndexEnable = true);


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
	D3D12_PRIMITIVE_TOPOLOGY_TYPE pipelinePrimitiveTopology_ = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;	//パイプライン
	D3D_PRIMITIVE_TOPOLOGY commandPrimitiveTopology_ = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;				//コマンドリスト

protected:
	const char* name = nullptr;

	//頂点データ
	VertexData* vertData_ = nullptr;
	UINT vertNum_ = 4;

	//インデックスデータ
	bool isIndexDataEnable_ = true;
	uint32_t* indexData_ = nullptr;
	UINT indexNum_ = 4;

	D3D12_FILL_MODE fillMode_ = D3D12_FILL_MODE_SOLID;	//塗りつぶし

	BlendSetting::BlendMode blendMode_ = BlendSetting::BlendMode::kBlendModeNormal;

	//パラメータ
	Vector4 color_ = {1.0f, 1.0f, 1.0f, 1.0f};
	bool isLightEnable_ = true;

	std::string texturePath_ = "uvChecker.png";

	std::string VSPath_ = "Object3D/Object3D.VS.hlsl";
	std::string PSPath_ = "Object3D/Object3D.PS.Texture.hlsl";

	//深度情報デバック用テクスチャのデスクリプタヒープ
	ShadowMapCommon* shadowCommon;
	DescriptorDSVData dhTexture_;
	ComPtr<ID3D12Resource> dhTextureResource_;
	uint32_t dhTextureHandle_;

public:
	BaseCollider* collider_ = nullptr;
};

