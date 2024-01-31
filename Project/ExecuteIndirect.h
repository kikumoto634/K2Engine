#pragma once
#include <wrl.h>
#include <vector>

#include "DirectXCommon.h"
#include "Pipeline.h"

#include "Camera.h"

class ExecuteIndirect
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	template <class T> using vector = std::vector<T>;

private:

	struct IndirectCommand{
		//転送するリソース
		D3D12_GPU_VIRTUAL_ADDRESS material;
		D3D12_GPU_VIRTUAL_ADDRESS wvp;
		D3D12_DRAW_ARGUMENTS drawArguments;
	};

public:
	ExecuteIndirect() = default;
	~ExecuteIndirect();

	void Initialize();
	void Update();
	void Draw(Camera* camera);

private:
	void CreatePipeline();
	void CreateVertex();
	void CreateMaterial();
	void CreateWVP();

private:
	DirectXCommon* dxCommon = nullptr;

	//パイプライン
	Pipeline* pipeline_ = nullptr;
	vector<D3D12_ROOT_PARAMETER> rootParameters_;
	vector<D3D12_INPUT_ELEMENT_DESC> inputElementDesc_;
	
	//リソース
	ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	Vector4* vertData_ = nullptr;
	const uint32_t vertNum = 1;

	ComPtr<ID3D12Resource> materialResource_;
	Vector4* materialData_ = nullptr;

	ComPtr<ID3D12Resource> wvpResource_;
	Matrix4x4* wvpData_ = nullptr;

	//コマンド個数(10万)
	const UINT kCommandNum = 10;
	//インスタンス数(10万)
	const UINT kInstanceNum = 10;

	//座標
	vector<Transform> transform;

	//コマンドシグネチャ
	ComPtr<ID3D12CommandSignature> commandSignature_;
	//コマンドバッファ
	ComPtr<ID3D12Resource> commandResource_;
};

