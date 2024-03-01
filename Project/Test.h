#pragma once
#include "TestCommon.h"

#include "Transform.h"
#include "GeometryDatas/Texture.h"
#include "GeometryDatas/VertexData.h"
#include "GeometryDatas/MaterialData.h"
#include "GeometryDatas/TransformationMatrixData.h"
#include "GeometryDatas/CameraForGPUData.h"


class Camera;
//幾何学オブジェクトの共通
class Test
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	virtual void Update();
	virtual void Draw(Camera* camera);

	//Getter/Setter
	//Transform GetTransform() const {return {translate,rotation,scale};}
protected:
	//初期化
	void Initialize(bool isIndexEnable = true);


	//リソース作成
	virtual void CreateVertex();
	virtual void CreateIndex();
	virtual void CreateMaterial();
	virtual void CreateWVP();
	virtual void CreateCamera();

	//パイプラインを新規作成したい場合、生成後、trueを返却する(呼び出しは不要)
	virtual bool PipelineCreate(){return false;}

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
	CameraForGPUData* cameraData_ = nullptr;

	//設定項目
	bool isLightEnable_ = true;
	Vector4 color_ = {1.0f, 1.0f, 1.0f, 1.0f};
	std::string texturePath_ = "uvChecker.png";

protected:
	//Instance
	DirectXCommon* dxCommon = nullptr;
	ID3D12GraphicsCommandList* cmdList = nullptr;

	//リソース関係
	ComPtr<ID3D12Resource> vertexResource_;
	ComPtr<ID3D12Resource> indexResource_;
	ComPtr<ID3D12Resource> materialResource_;
	ComPtr<ID3D12Resource> wvpResource_;
	ComPtr<ID3D12Resource> cameraResource_;

	//インスタンシング描画用データ構造
	int X = 10;
	int Y = 10;
	int Z = 350;
	const int kNumMaxInstance_ = X*Y*Z;
	std::vector<Transform> trans;

	//インスタンシング描画用ハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE materialInstancingCPU_;
	D3D12_GPU_DESCRIPTOR_HANDLE materialInstancingGPU_;

	D3D12_CPU_DESCRIPTOR_HANDLE wvpInstancingCPU_;
	D3D12_GPU_DESCRIPTOR_HANDLE wvpInstancingGPU_;
};

