#pragma once
#include <wrl.h>
#include <dxcapi.h>

#include "Transform.h"
#include "Texture.h"

#include "VertexData.h"
#include "MaterialData.h"
#include "TransformationMatrixData.h"

#include "GeometryBaseCommon.h"

//幾何学オブジェクトの共通
class GeometryBase
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	template <class T> using vector = std::vector<T>;

public:
	static void StaticInitialize();
	static void StaticDraw();

public:
	virtual void Draw(Matrix4x4 viewProjectionMatrix);

protected:
	//初期化
	void Initialize(bool isIndexEnable = true);

private:
	//頂点リソース/ビュー
	void CreateVertex();
	//インデックスリソース/ビュー
	void CreateIndex();
	//定数リソース/ビュー
	void CreateMaterial();
	//行列リソース/ビュー
	void CreateWVP();

private:
	static std::unique_ptr<GeometryBaseCommon> common;

private:
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

protected:
	//頂点データ
	VertexData* vertData_ = nullptr;
	UINT vertNum_ = 4;

	//インデックスデータ
	bool isIndexDataEnable_ = true;
	uint32_t* indexData_ = nullptr;
	UINT indexNum_ = 4;

	//パラメータ
	Transform transform;
	Vector4 color_ = {1.0f, 1.0f, 1.0f, 1.0f};
	bool isLightEnable = true;

	std::string texturePath_ = "uvChecker.png";
};

