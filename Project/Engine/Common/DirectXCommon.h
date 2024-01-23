#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <cassert>
#include <chrono>
#include <wrl.h>

#include "WindowsApp.h"
#include "Vector4.h"

#include "DescriptorData.h"

class DirectXCommon
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

private:
	struct D3DResourceLeakChacker{
		~D3DResourceLeakChacker(){
			//リソースチェック
			ComPtr<IDXGIDebug1> debugChuck;
			if(SUCCEEDED(DXGIGetDebugInterface1(0,IID_PPV_ARGS(&debugChuck)))){
				debugChuck->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
				debugChuck->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
				debugChuck->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
			}
		}
	};

public:
	static DirectXCommon* GetInstance();
	static DirectXCommon* Create(Vector4 clearColor = {0.1f, 0.2f, 0.5f, 1.0f});
	static void Finalize()	{delete instance_;}

public:
	DirectXCommon() = default;
	~DirectXCommon(){
		CloseHandle(fenceEvent_);
	}

	void PreDraw();
	void PostDraw();

	//Getter
	ID3D12Device* GetDevice()	{return device_.Get();}
	ID3D12GraphicsCommandList* GetCommandList()	{return commandList_.Get();}
	ID3D12CommandQueue* GetCommandQueue()	{return commandQueue_.Get();}
	ID3D12CommandAllocator*	GetCommandAllocator()	{return commandAllocator_.Get();}

	DescriptorSRVData& GetSRVDescriptorHeap()	{return srvDescriptorHeap_;}

	uint32_t GetDescriptorSizeSRV()	{return descriptorSizeSRV_;}

	DXGI_SWAP_CHAIN_DESC1 GetSwapChainDesc()	{return swapChainDesc;}

	D3D12_RENDER_TARGET_VIEW_DESC GetRTVDesc()	{return rtvDesc;}

	D3D12_VIEWPORT GetViewport()	{return viewport_;}
	D3D12_RECT GetScissorRect()	{return scissorRect_;}

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

#pragma region RTV/SRV/DSV : Resource, View系
	//RTV用のデスクリプタヒープ作成
	bool CreateRTVDescriptorHeap();
	//SRV用のデスクリプタヒープ作成
	bool CreateSRVDescriptorHeap();
	//DSV用のデスクリプタヒープ作成
	bool CreateDSVDescriptorHeap();

	//SwapChainからResourceを引っ張る
	bool BringResourceFromSwapChain();
	//引っ張ってきたResourceに対してDescriptor上にRTVを作成(スワップチェーン)
	bool CreateRTV();
	//指定したサイズにて深度関連を乗せたResourceを作成
	bool CreateDSV();
#pragma endregion

#pragma region フェンス
	bool CreateFence();
#pragma endregion

#pragma region ViewportとScissor
	bool CreateViewport();
	bool CreateScissor();
#pragma endregion


	void InitializeFixFPS();
	void UpdateFixFPS();

private:
	//リソースチェック用
	static D3DResourceLeakChacker leakCheck;
	static DirectXCommon* instance_;
	static float clearColor_[4];
	//ダブルバッファ
	static const int SwapChainNum = 2;
	
public:
	//FPS
	static double fps_;
	static float fixedFpsValue_;

private:
	//SRV最大数
	const int kSRVNumMax = 128;

private:
	HRESULT result = {};
	WindowsApp* windows = nullptr;

#ifdef _DEBUG
	//デバックレイヤー
	ComPtr<ID3D12Debug1> debugController_;
#endif

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
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	ComPtr<IDXGISwapChain4> swapChain_;


	//RTV : Resourceに対して描く作業をするもの (ダブルバッファリングなので今回は2つ)
	//SRV : Resourceに対して見る作業をするもの
	//DSV : Resourceの深度に関する情報を格納するもの
	//ディスクリプタヒープとは、作業方法(View)の情報を格納する配列
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	//RTVのディスクリプタヒープ(RTVのヒープは全体で一つだけ)
	DescriptorRTVData rtvDescriptorHeap_;
	//SRV : Resourceに対して見る作業をするもの(SRVのヒープは全体で一つだけ)
	DescriptorSRVData srvDescriptorHeap_;
	//DSV用のディスクリプタヒープ	(DVSのヒープは全体に一つだけ)　
	DescriptorDSVData dsvDescriptorHeap_;

	//各ディスクリプタヒープサイズ
	//RTV
	uint32_t descriptorSizeRTV_;
	//SRV
	uint32_t descriptorSizeSRV_;
	//DSV
	uint32_t descriptorSizeDSV_;

	//SwapChainのResource
	ComPtr<ID3D12Resource> swapChainResources_[SwapChainNum];
	//RTVは二つ生成するのでディスクリプタハンドルを2つ用意(スワップチェーン用)
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[SwapChainNum];


	//フェンス
	ComPtr<ID3D12Fence> fence_;
	uint64_t fenceValue_ = 0;
	HANDLE fenceEvent_;


	//ビューポート
	D3D12_VIEWPORT viewport_{};
	//シザー矩形
	D3D12_RECT scissorRect_{};


	std::chrono::steady_clock::time_point reference_;
};

