#pragma once
#include "Transform.h"
#include "Pipeline.h"

#include "VertexData.h"
#include "MaterialData.h"
#include "TransformationMatrixData.h"
#include "DirectionalLightData.h"

class Object3D
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	template <class T> using vector = std::vector<T>;

public:
	static Object3D* Create();

public:
	~Object3D()	{
		delete pipeline_;
	}
	//更新
	void Update();
	//描画
	void Draw(Matrix4x4 viewProjectionMatrix);

private:
	void Initialize();
	void PipelineInitialize();

#pragma region Resource / View / Heap
	//ResourceとHeap
	// Resourceはメモリ上で配置されるが、「どこ」のメモリに配置するのかは重要
	// ・GPUでよく使用するものはGPUに近いほど良い
	// ・CPUでよく使用するものはCPUに近いほど良い
	
	// これらを全体的に管理するのが、「Heap」 
	// Resourceはどの種類のHeap上に配置するのか決めてから作成する
	// ・Default	(デフォルト)
	// ・Upload		(アップロード)
	// ・Readback	(リードバック)

	/// <summary>
	/// リソース作成関数
	/// </summary>
	/// <param name="sizeInByte"> 使用するバイトサイズ </param>
	ID3D12Resource* CreateBufferResource(size_t sizeInByte);
	
	/// <summary>
	/// 頂点ビュー作成関数
	/// </summary>
	/// <param name="sizeInByte">使用するバイトサイズ</param>
	/// <param name="Num">使用する個数</param>
	void CreateBufferView(D3D12_VERTEX_BUFFER_VIEW& view, ID3D12Resource* resource, UINT sizeInByte, UINT strideInBytes);
	/// <summary>
	/// インデックスビュー作成関数
	/// </summary>
	/// <param name="sizeInByte">使用するバイトサイズ</param>
	/// <param name="Num">使用する個数</param>
	void CreateBufferView(D3D12_INDEX_BUFFER_VIEW& view, ID3D12Resource* resource, UINT sizeInByte);

#pragma region 頂点リソース/ビュー/更新
	// 頂点データ用のResource : VertexBuffer/VertexResource		※メモリ内に保存されているデータ
	// 頂点データ用のView     : VertexBufferView(VBV)			※Shaderへの入力頂点として処理する作業方法
	bool CreateVertex();
#pragma endregion

#pragma region インデックスリソース/ビュー
	//インデックス用のResource
	bool CreateIndex();
#pragma endregion

#pragma region 定数リソース/更新
	// 定数データ用のResource : ConstBuffer/ConstResource	
	bool CreateConstant();
#pragma endregion

#pragma region 行列リソース
	//移動の行列用データのResource
	bool CreateWVP();
#pragma endregion

#pragma region ライトリソース
	//平行光源用データのResource
	bool CreateDirectionalLight();
#pragma endregion

private:
	HRESULT result{};
	DirectXCommon* dxCommon_ = nullptr;

	//パイプライン
	Pipeline* pipeline_= nullptr;
	vector<D3D12_ROOT_PARAMETER> rootParameters_;			//ルートパラメータ
	vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescs_;	//インプットレイアウト
	vector<D3D12_STATIC_SAMPLER_DESC> staticSamplers_;		//サンプラー

	//頂点リソース
	ComPtr<ID3D12Resource> vertexResource_;
	VertexData* vertexData = nullptr; 
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	//インデックスリソース
	ComPtr<ID3D12Resource> indexResource_{};
	uint32_t* indexData_ = nullptr;
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};

	//定数リソース
	ComPtr<ID3D12Resource> constResource_;
	GeometryMaterial* materialData = nullptr;

	//行列リソース
	ComPtr<ID3D12Resource> wvpResource_;
	TransformationMatrix* wvpData = nullptr;

	//Lightリソース
	ComPtr<ID3D12Resource> directionalLightResource_;
	DirectionalLightData* directionalLightData = nullptr;


	//球体分割数
	const uint32_t kSubdivision = 16;
	//色
	Vector4 color_ = {1,1,1,1};
	//トランスフォーム情報
	Transform transform_ =  {{0,0,0}, {0,0,0}, {0.5,0.5,0.5}};
	bool isUseMonsterBall = true;

	//SRVのDescriptorTableの先頭(テクスチャ)
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU1_ = {};
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU2_ = {};



	//Sprite用
	ComPtr<ID3D12Resource> vertexResourceSprite_;
	VertexData* vertexDataSprite_ = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite_{};

	ComPtr<ID3D12Resource> indexResourceSprite_{};
	uint32_t* indexDataSprite_ = nullptr;
	D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite{};

	ComPtr<ID3D12Resource> constResourceSprite_;
	GeometryMaterial* materialDataSprite = nullptr;

	ComPtr<ID3D12Resource> transformationMatrixResourceSprite_;
	TransformationMatrix* transformationMatrixDataSprite_ = nullptr;

	Transform transformSprite_ = {{0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {1.0f,1.0f,1.0f}};
	Transform uvTransformSprite{
		{0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 0.0f},
		{1.0f, 1.0f, 1.0f},
	};
};

