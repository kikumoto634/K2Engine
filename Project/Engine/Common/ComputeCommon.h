#pragma once
#include "DirectXCommon.h"

//コンピュートシェーダ用
class ComputeCommon
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	void Initialize();
	//計算実行
	void Excution();

private:
	void CreateRootSignature();
	void CreatePipeline();

	void CreateCommand();

	void CreateComputeDescriptorHeap();
	void CreateResource();
	void CreateUAV();

	void CreateFence();


public:
	DirectXCommon* dxCommon = nullptr;

	//ルートシグネチャ
	ComPtr<ID3D12RootSignature> rootSignature_;
	
	//シェーダ
	ComPtr<ID3DBlob> computeShaderBlob_;
	
	//パイプライン
	ComPtr<ID3D12PipelineState> pipeline_;

	//コマンドキュー
	ComPtr<ID3D12CommandQueue> commandQueue_;
	//コマンドアロケータ
	ComPtr<ID3D12CommandAllocator> commandAllocator_;
	//コマンドリスト
	ComPtr<ID3D12GraphicsCommandList> commandList_;

	//DescriptorHeap
	ComPtr<ID3D12DescriptorHeap> descriptorHeap_;
	//作成リソース
	ComPtr<ID3D12Resource> resource_;

	//送受信用データ
	void* data;


	ComPtr<ID3D12Fence> fence_;
	uint64_t fenceValue_ = 0;
	HANDLE fenceEvent_;
};

