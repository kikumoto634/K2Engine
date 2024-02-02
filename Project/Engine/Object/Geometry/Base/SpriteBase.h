#pragma once
#include <wrl.h>

#include "DirectXCommon.h"
#include "Geometry/Common/SpriteCommon.h"

#include "Transform.h"
#include "Texture.h"
#include "Camera.h"

#include "VertexData.h"
#include "MaterialData.h"
#include "TransformationMatrixData.h"

class SpriteBase : public Transform
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	template <class T> using vector = std::vector<T>;

public:
	~SpriteBase(){}
	void Draw(Camera* camera);


protected:
	//初期化
	void Initialize(bool isIndexEnable = true);

	void ApplyGlobalVariablesInitialize();
	void ApplyGlobalVariablesUpdate();

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
	//Instance
	DirectXCommon* dxCommon = nullptr;

	//テクスチャ情報
	Texture texture_;

	//リソース関係
	ComPtr<ID3D12Resource> vertexResource_;		//頂点
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;

	ComPtr<ID3D12Resource> indexResource_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_;

	ComPtr<ID3D12Resource> constResource_;		//定数
	TextureMaterial* materialData_ = nullptr;

	ComPtr<ID3D12Resource> wvpResource_;		//行列
	TransformationMatrix* wvpData_ = nullptr;

protected:
	Transform uvTransformSprite_{
		{0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 0.0f},
		{1.0f, 1.0f, 1.0f},
	};
	Vector2 textureSize_{};

	//頂点データ
	VertexData* vertData_ = nullptr;
	UINT vertNum_ = 4;

	//インデックスデータ
	uint32_t* indexData_ = nullptr;
	UINT indexNum_ = 6;

	//パラメータ
	Vector4 color_ = {1.0f, 1.0f, 1.0f, 1.0f};
	std::string texturePath_ = "white1x1.png";

	
	//共有処理を変更する場合の設定
};

