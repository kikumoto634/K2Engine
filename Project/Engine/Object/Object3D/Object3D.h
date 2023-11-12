#pragma once
#include "Transform.h"
#include "Pipeline.h"

class Object3D
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	template <class T> using vector = std::vector<T>;

	struct VertexData{
		Vector4 position;
		Vector2 texcoord;
	};

public:
	static Object3D* Create();

public:
	~Object3D()	{
		delete pipeline_;
	}
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
	/// ビュー作成関数
	/// </summary>
	/// <param name="sizeInByte">使用するバイトサイズ</param>
	/// <param name="Num">使用する個数</param>
	void CreateBufferView(D3D12_VERTEX_BUFFER_VIEW& view, ID3D12Resource* resource, UINT sizeInByte, UINT strideInBytes);

#pragma region 頂点リソース/ビュー/更新
	// 頂点データ用のResource : VertexBuffer/VertexResource		※メモリ内に保存されているデータ
	// 頂点データ用のView     : VertexBufferView(VBV)			※Shaderへの入力頂点として処理する作業方法
	bool CreateVertex();
#pragma endregion

#pragma region 定数リソース/更新
	// 定数データ用のResource : ConstBuffer/ConstResource	
	bool CreateConstant();
#pragma endregion

#pragma region 行列リソース
	//移動の行列用データのResource
	bool CreateWVP();
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

	//定数リソース
	ComPtr<ID3D12Resource> constResource_;
	Vector4* materialData = nullptr;

	//行列リソース
	ComPtr<ID3D12Resource> wvpResource_;
	Matrix4x4* wvpData = nullptr;


	//球体分割数
	const uint32_t kSubdivision = 16;
	//色
	Vector4 color_ = {1,1,1,1};
	//トランスフォーム情報
	Transform transform_ =  {{0,0,0}, {0,0,0}, {1,1,1}};

	//SRVのDescriptorTableの先頭(テクスチャ)
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_ = {};



	//Sprite用
	ComPtr<ID3D12Resource> vertexResourceSprite_;
	VertexData* vertexDataSprite_ = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite_{};

	ComPtr<ID3D12Resource> transformationMatrixResourceSprite_;
	Matrix4x4* transformationMatrixDataSprite_ = nullptr;

	Transform transformSprite_ = {{0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {1.0f,1.0f,1.0f}};
};

