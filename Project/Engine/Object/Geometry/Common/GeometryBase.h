#pragma once
#include <wrl.h>
#include <dxcapi.h>
#include <vector>

#include "Pipeline.h"
#include "DirectXCommon.h"
#include "Transform.h"

#include "Vector3.h"
#include "Vector2.h"
#include "Matrix4x4.h"

//幾何学オブジェクトの共通
class GeometryBase
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	template <class T> using vector = std::vector<T>;

private:
	//頂点データ
	struct VertexData{
		Vector4 position;	//座標
		Vector2 texcoord;	//UV
		Vector3 normal;		//法線
	};

	//マテリアルデータ
	struct Material{
		Vector4 color;			//色
		int enableLighting;		//ライト処理可否
		float padding[3];		//パディング
		Matrix4x4 uvTransform;	//UV行列
	};

	//行列
	struct TransformationMatrix{
		Matrix4x4 WVP;		//ワールドビュープロジェクション
		Matrix4x4 World;	//ワールド
	};

	//平行高原
	struct DirectionalLight{
		Vector4 color;
		Vector3 direction;
		float intensity;
	};

public:
	~GeometryBase(){
		delete pipeline_;
	}
	void Draw(Matrix4x4 viewProjectionMatrix);

protected:
	//初期化
	void Initialize();

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
	//ライトリソース/ビュー
	void CreateLight();

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

	ComPtr<ID3D12Resource> constResource_;		//定数
	Material* materialData_ = nullptr;

	ComPtr<ID3D12Resource> wvpResource_;		//行列
	TransformationMatrix* wvpData_ = nullptr;

	ComPtr<ID3D12Resource> directionalLightResource_;	//ライト
	DirectionalLight* directionalLightData_ = nullptr;


	//テクスチャ関係
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU_ = {};	//画像のテクスチャハンドルCPU
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_ = {};	//画像テクスチャハンドルGPU


protected:
	//頂点データ
	VertexData* vertData_ = nullptr;
	UINT vertNum_ = 4;

	//パラメータ
	Transform transform_ = {{0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {1.0f,1.0f,1.0f}};	//座標
	Vector4 color_ = {1.0f, 1.0f, 1.0f, 1.0f};
	bool isLightEnable = true;

	Vector4 lightColor_ = {1,1,1,1};
	Vector3 lightDirection_ = {0,-1,0};
	float lightIntensity = 1.0f;

	std::string texturePath_ = "uvChecker.png";
};

