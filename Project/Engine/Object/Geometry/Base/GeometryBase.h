#pragma once
#include "Geometry/Common/BaseCollider.h"

#include "Geometry/Common/GeometryCommon.h"

#include "Transform.h"
#include "GeometryDatas/Texture.h"
#include "GeometryDatas/VertexData.h"
#include "GeometryDatas/MaterialData.h"
#include "GeometryDatas/TransformationMatrixData.h"


class Camera;
//幾何学オブジェクトの共通
class GeometryBase : public Transform
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	virtual void Update();
	virtual void Draw(Camera* camera);
	virtual void OnCollision(const CollisionInfo& info) {}

	//Getter/Setter
	Transform GetTransform() const {return {translate,rotation,scale};}
	BaseCollider* GetCollider()	{return collider_;}

protected:
	//初期化
	void Initialize(bool isIndexEnable = true);


	//リソース作成
	virtual void CreateVertex();
	virtual void CreateIndex();
	virtual void CreateMaterial();
	virtual void CreateWVP();

	//グローバル設定
	virtual void ApplyGlobalVariablesInitialize();
	virtual void ApplyGlobalVariablesUpdate();

public:
	//コライダー
	BaseCollider* collider_ = nullptr;

protected:
	const char* name = nullptr;
	
	//テクスチャ情報
	Texture texture_;

	//リソース関係
	VertexData* vertData_ = nullptr;
	UINT vertNum_ = 4;
	
	bool isIndexDataEnable_ = true;
	uint32_t* indexData_ = nullptr;
	UINT indexNum_ = 4;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_;
	GeometryMaterial* materialData_ = nullptr;
	TransformationMatrix* wvpData_ = nullptr;

	//設定項目
	bool isLightEnable_ = true;
	Vector4 color_ = {1.0f, 1.0f, 1.0f, 1.0f};
	std::string texturePath_ = "uvChecker.png";

private:
	//Instance
	DirectXCommon* dxCommon = nullptr;

	//リソース関係
	ComPtr<ID3D12Resource> vertexResource_;
	ComPtr<ID3D12Resource> indexResource_;
	ComPtr<ID3D12Resource> materialResource_;
	ComPtr<ID3D12Resource> wvpResource_;
};

