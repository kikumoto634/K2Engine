#pragma once
#include <wrl.h>
#include <dxcapi.h>
#include <vector>
#include <list>

#include "DirectXCommon.h"

#include "Transform.h"
#include "GeometryDatas/Texture.h"
#include "GeometryDatas/VertexData.h"
#include "GeometryDatas/MaterialData.h"
#include "ParticleDatas/ParticleData.h"
#include "ParticleDatas/ParticleForGPUData.h"

#include <random>

class Camera;
class ParticleBase
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	template <class T> using vector = std::vector<T>;
	template <class T> using list = std::list<T>;
public:
	~ParticleBase(){}

	virtual void Update();
	virtual void Draw(Camera* camera);

	virtual void Add(const Vector3& translate = {0,0,0});
protected:
	//初期化
	virtual void Initialize(bool isIndexEnable = true);

	virtual ParticleData MakeNewParticle(std::mt19937 &randomEngine, const Vector3& translate);

	virtual void ApplyGlobalVariablesInitialize();
	virtual void ApplyGlobalVariablesUpdate();

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

protected:
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

	//パラメータ
	//最大数
	int kNumMaxInstance_ = 100;
	//パーティクル変数
	list<ParticleData> particles_;

	const float kDeltaTime_ = 1.0f/60.f;

	std::string texturePath_ = "uvChecker.png";
};

