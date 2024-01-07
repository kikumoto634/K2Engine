#pragma once
#include <wrl.h>
#include <vector>

#include "Transform.h"
#include "Pipeline.h"
#include "DirectXCommon.h"
#include "Camera.h"

class GeometryBaseCollider : public Transform
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	template <class T> using vector = std::vector<T>;
public:
	~GeometryBaseCollider(){
		delete pipeline_;
	}
	void Draw(Camera* camera);

	//Getter/Setter
	Transform GetTransform() const {return {translate,rotation,scale};}

protected:
	//初期化
	void Initialize(bool isIndexEnable = true);

private:
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

	//リソース関係
	ComPtr<ID3D12Resource> vertexResource_;		//頂点
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;

	ComPtr<ID3D12Resource> indexResource_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_;

	ComPtr<ID3D12Resource> constResource_;		//定数
	Vector4* materialData_ = nullptr;

	ComPtr<ID3D12Resource> wvpResource_;		//行列
	Matrix4x4* wvpData_ = nullptr;

	//描画方法
	D3D12_PRIMITIVE_TOPOLOGY_TYPE pipelinePrimitiveTopology_ = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;	//パイプライン
	D3D_PRIMITIVE_TOPOLOGY commandPrimitiveTopology_ = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;				//コマンドリスト


protected:
	const char* name_ = "";

	//頂点データ
	Vector4* vertData_ = nullptr;
	UINT vertNum_ = 4;

	//インデックスデータ
	uint32_t* indexData_ = nullptr;
	UINT indexNum_ = 4;

	D3D12_FILL_MODE fillMode_ = D3D12_FILL_MODE_WIREFRAME;	//塗りつぶし

	//パラメータ
	Vector4 color_ = {0.1f, 0.5f, 0.1f, 1.0f};


	std::string VSPath_ = "Collider/Collider.VS.hlsl";
	std::string PSPath_ = "Collider/Collider.PS.hlsl";
};

