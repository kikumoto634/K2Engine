#pragma once
#include <wrl.h>
#include <vector>

#include "Transform.h"
#include "Pipeline.h"
#include "DirectXCommon.h"
#include "Camera.h"

class EmitterFrameBase : public Transform
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	template <class T> using vector = std::vector<T>;
public:
	~EmitterFrameBase(){};
	void Draw(Camera* camera);

protected:
	//初期化
	void Initialize(bool isIndexEnable = true);
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

	//リソース関係
	ComPtr<ID3D12Resource> vertexResource_;		//頂点
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;

	ComPtr<ID3D12Resource> indexResource_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_;

	ComPtr<ID3D12Resource> constResource_;		//定数
	Vector4* materialData_ = nullptr;

	ComPtr<ID3D12Resource> wvpResource_;		//行列
	Matrix4x4* wvpData_ = nullptr;

protected:
	//頂点データ
	Vector4* vertData_ = nullptr;
	UINT vertNum_ = 4;

	//インデックスデータ
	uint32_t* indexData_ = nullptr;
	UINT indexNum_ = 4;

	//パラメータ
	Vector4 color_ = {0.1f, 0.5f, 0.1f, 1.0f};
};
