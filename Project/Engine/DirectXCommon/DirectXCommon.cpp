#include "DirectXCommon.h"

#include <d3dx12.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

DirectXCommon* DirectXCommon::instance_ = nullptr;
float DirectXCommon::clearColor_[4] = {0.1f, 0.25f, 0.5f, 1.0f};

DirectXCommon *DirectXCommon::Create(Vector4 clearColor)
{
	if(!instance_){
		instance_ = new DirectXCommon();
		instance_->Initialize();

		instance_->clearColor_[0] = clearColor.x;
		instance_->clearColor_[1] = clearColor.y;
		instance_->clearColor_[2] = clearColor.z;
		instance_->clearColor_[3] = clearColor.w;
	}
	return instance_;
}

void DirectXCommon::Initialize()
{
	windows = WindowsApp::GetInstance();

	//デバック
	assert(SUCCEEDED(CreateDebugLayer()));

	//ファクトリー・アダプタ・デバイス
	assert(SUCCEEDED(CreateFactory()));
	assert(SUCCEEDED(CreateAdapter()));
	assert(SUCCEEDED(CreateDevice()));

	//エラー
	assert(SUCCEEDED(CreateErrorInfoQueue()));

	//コマンド系
	assert(SUCCEEDED(CreateCommandQueue()));
	assert(SUCCEEDED(CreateCommandAllocator()));
	assert(SUCCEEDED(CreateCommandList()));

	//スワップチェーン
	assert(SUCCEEDED(CreateSwapChain()));

	//RTV
	assert(SUCCEEDED(CreateRTVDescriptorHeap()));
	assert(SUCCEEDED(BringResourceFromSwapChain()));
	assert(SUCCEEDED(CreateRTV()));
}

void DirectXCommon::Draw()
{
	//書き込むバックばっふのインデックス取得
	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

	//描画先のRTVを設定
	commandList_->OMSetRenderTargets(1, &rtvHandles_[backBufferIndex], false, nullptr);
	
	//指定した色で画面をクリア
	commandList_->ClearRenderTargetView(rtvHandles_[backBufferIndex], clearColor_, 0, nullptr);

	//コマンドリストの内容を確定。
	result = commandList_->Close();
	assert(SUCCEEDED(result));

	//GPUにコマンドリストの実行を行わせる
	ID3D12CommandList* commandList[] = {commandList_.Get()};
	commandQueue_->ExecuteCommandLists(1, commandList);

	//GPUとOSに画面の交換
	swapChain_->Present(1,0);

	//次のフレーム用のコマンドリスト用意
	result = commandAllocator_.Get()->Reset();
	assert(SUCCEEDED(result));
	result = commandList_->Reset(commandAllocator_.Get(), nullptr);
	assert(SUCCEEDED(result));
}


#pragma region デバック系
bool DirectXCommon::CreateDebugLayer()
{
#ifdef _DEBUG
	//デバックレイヤー
	ID3D12Debug1* debugController_ = nullptr;

	if(SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController_)))){
		//デバックレイヤー有効化
		debugController_->EnableDebugLayer();
		//さらにGPU側でもチェックを行う
		debugController_->SetEnableGPUBasedValidation(true);
	}
#endif
	return true;
}

bool DirectXCommon::CreateErrorInfoQueue()
{
#ifdef _DEBUG
	//エラー際　停止
	ID3D12InfoQueue* infoQueue_ = nullptr;

	if(SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue_)))){
		//やばいエラー時に止まる
		infoQueue_->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		//エラー時に止まる
		infoQueue_->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		//警告時に止まる
		infoQueue_->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
		//解放
		infoQueue_->Release();

		//抑制するメッセージのID
		D3D12_MESSAGE_ID denyIds[] = {
			//Windows11でのDXGIUデバックレイヤーとDX12デバックレイヤーの相互作用バグによるエラーメッセージ
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};

		//抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = {D3D12_MESSAGE_SEVERITY_INFO};
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		//指定したメッセージの表示を抑制
		infoQueue_->PushStorageFilter(&filter);
	}
#endif
	return true;
}
#pragma endregion


#pragma region デバイス系
bool DirectXCommon::CreateFactory()
{
	//ファクトリー作成
	result = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
	assert(SUCCEEDED(result));

	return true;
}

bool DirectXCommon::CreateAdapter()
{
	//良い順にアダプタを頼む
	for(UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(i,DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter_)) != DXGI_ERROR_NOT_FOUND; ++i){
		//アダプタの情報取得
		DXGI_ADAPTER_DESC3 adapterDesc{};
		result = useAdapter_->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(result));

		//ソフトウェアアダプタがなければ採用
		if(!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)){
			//採用したアダプタの情報をログに出力
			windows->Log(std::format("Use Adapter : {}\n", (char*)adapterDesc.Description));
			break;
		}
		//ソフトウェアアダプタの場合見なかったことにする
		useAdapter_ = nullptr;
	}
	//適切アダプタが見つからない場合起動なし
	assert(useAdapter_ != nullptr);

	return true;
}

bool DirectXCommon::CreateDevice()
{
	//機能レベルとログ出力用の文字列
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2, D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0
	};
	const char* featureLevelStrings[] = {"12_2", "12_1", "12_0"};
	//高い順に生成できるか
	for(size_t i = 0; i < _countof(featureLevels); i++){
		//採用したアダプタでデバイス生成
		result = D3D12CreateDevice(useAdapter_.Get(), featureLevels[i], IID_PPV_ARGS(&device_));
		if(SUCCEEDED(result)){
			//生成できたのでログ出力
			windows->Log(std::format("FeatureLevels : {}\n", featureLevelStrings[i]));
			break;
		}
	}

	//デバイス生成がうまくできない場合起動なし
	assert(device_ != nullptr);
	windows->Log("Complete create D3D12Device\n");

	return true;
}
#pragma endregion


#pragma region コマンド系
bool DirectXCommon::CreateCommandQueue()
{
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	result = device_->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue_));
	assert(SUCCEEDED(result));

	return true;
}

bool DirectXCommon::CreateCommandAllocator()
{
	result = device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));
	assert(SUCCEEDED(result));

	return true;
}

bool DirectXCommon::CreateCommandList()
{
	result = device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr, IID_PPV_ARGS(&commandList_));
	assert(SUCCEEDED(result));

	return true;
}
#pragma endregion


#pragma region スワップチェーン系
bool DirectXCommon::CreateSwapChain()
{
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = WindowsApp::kWindowWidth_;	//画面幅
	swapChainDesc.Height = WindowsApp::kWindowHeight_;	//画面高さ
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	//色系形式
	swapChainDesc.SampleDesc.Count = 1;	//マルチサンプル
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	//描画ターゲットとして利用
	swapChainDesc.BufferCount = SwapChainNum;	//バッファ数
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	//モニタに写したら中身を破棄

	//コマンドキュー、ウィンドウハンドル設定で生成
	result = dxgiFactory_->CreateSwapChainForHwnd(commandQueue_.Get(), windows->GetHWND(), &swapChainDesc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(swapChain_.GetAddressOf()));
	assert(SUCCEEDED(result));

	return true;
}
#pragma endregion


#pragma region RTV : Resource, View系
bool DirectXCommon::CreateRTVDescriptorHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc{};
	rtvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;	//レンダリングターゲットビュー用
	rtvDescriptorHeapDesc.NumDescriptors = SwapChainNum;	//ダブルバッファ用(ディスクリプタの数)

	//ディスクリプタヒープ作成
	result = device_->CreateDescriptorHeap(&rtvDescriptorHeapDesc, IID_PPV_ARGS(&rtvDescriptorHeap_));
	assert(SUCCEEDED(result));

	return true;
}

bool DirectXCommon::BringResourceFromSwapChain()
{
	//SwapChainからResourceを引っ張ってくる
	for(int i = 0; i < SwapChainNum; i++){
		result = swapChain_->GetBuffer(i, IID_PPV_ARGS(&swapChainResources_[i]));
		assert(SUCCEEDED(result));
	}

	return true;
}

bool DirectXCommon::CreateRTV()
{
	//RTV設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;	//出力結果をSRGBに変換、書き込み
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;	//2Dテクスチャとして書き込む
	
	//ディスクリプタの先頭アドレスを取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();

	for(int i = 0; i < SwapChainNum; i++){
		
		//作成する場所指定
		rtvHandles_[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(
			rtvStartHandle, i,
			device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
		);
		
		//生成
		device_->CreateRenderTargetView(swapChainResources_[i].Get(), &rtvDesc, rtvHandles_[i]);
	}

	return true;
}
#pragma endregion