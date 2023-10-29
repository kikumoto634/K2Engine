#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>

#include <wrl.h>

#include "WindowsApp.h"
#include "Vector4.h"

class DirectXCommon
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	static DirectXCommon* Create(Vector4 clearColor = {0.1f, 0.2f, 0.5f, 1.0f});


public:
	void Draw();

private:
	void Initialize();

#pragma region デバック系
	//デバックレイヤー
	bool CreateDebugLayer();
	//エラー際　停止
	bool CreateErrorInfoQueue();
#pragma endregion

#pragma region デバイス系
	//ファクトリー
	bool CreateFactory();
	//アダプタ
	bool CreateAdapter();
	//デバイス
	bool CreateDevice();
#pragma endregion

#pragma region コマンド系
	bool CreateCommandQueue();
	bool CreateCommandAllocator();
	bool CreateCommandList();
#pragma endregion

#pragma region スワップチェーン系
	bool CreateSwapChain();
#pragma endregion

#pragma region RTV : Resource, View系
	bool CreateRTVDescriptorHeap();
	bool BringResourceFromSwapChain();
	bool CreateRTV();
#pragma endregion

#pragma region フェンス
	bool CreateFence();
#pragma endregion


private:
	static DirectXCommon* instance_;

	static float clearColor_[4];

private:
	static const int SwapChainNum = 2;

private:
	HRESULT result = {};
	WindowsApp* windows = nullptr;

	//DXGIファクトリー
	ComPtr<IDXGIFactory7> dxgiFactory_;
	//アダプタ
	ComPtr<IDXGIAdapter4> useAdapter_;
	//デバイス
	ComPtr<ID3D12Device> device_;

	//コマンドキュー
	ComPtr<ID3D12CommandQueue> commandQueue_;			//命令をGPUに送り込む
	//コマンドアロケータ
	ComPtr<ID3D12CommandAllocator> commandAllocator_;	//命令保存用のメモリ管理機構(Listに必須)
	//コマンドリスト
	ComPtr<ID3D12GraphicsCommandList> commandList_;		//命令群をまとめる

	//スワップチェーン
	ComPtr<IDXGISwapChain4> swapChain_;


	//RTV : Resourceに対して描く作業をするもの (ダブルバッファリングなので今回は2つ)
	//SRV : Resourceに対して見る作業をするもの
	//ディスクリプタヒープとは、作業方法(View)の情報を格納する配列

	//RTVのディスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_;
	//SwapChainのResource
	ComPtr<ID3D12Resource> swapChainResources_[SwapChainNum];
	//RTVは二つ生成するのでディスクリプタを2つ用意
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[SwapChainNum];

	//フェンス
	ComPtr<ID3D12Fence> fence_;
	uint64_t fenceValue_ = 0;
	HANDLE fenceEvent_;
};

