#include "DirectXCommon.h"
#include <thread>
#include <d3dx12.h>
#include "DescriptorHeap.h"
#include "DepthStencil.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

DirectXCommon* DirectXCommon::instance_ = nullptr;
float DirectXCommon::clearColor_[4] = {0.1f, 0.25f, 0.5f, 1.0f};

double DirectXCommon::fps_ = 0.0f;
float DirectXCommon::fixedFpsValue_ = 120.0f;

DirectXCommon *DirectXCommon::GetInstance()
{
	return instance_;
}

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

	InitializeFixFPS();

	//デバック
	CreateDebugLayer();

	//ファクトリー・アダプタ・デバイス
	CreateFactory();
	CreateAdapter();
	CreateDevice();

	//エラー
	CreateErrorInfoQueue();

	//コマンド系
	CreateCommandQueue();
	CreateCommandAllocator();
	CreateCommandList();

	//スワップチェーン
	CreateSwapChain();

	//RTV/SRV/DSV
	CreateRTVDescriptorHeap();
	CreateSRVDescriptorHeap();
	CreateDSVDescriptorHeap();

	BringResourceFromSwapChain();
	CreateRTV();
	CreateDSV();

	//フェンス
	CreateFence();

	//ビューポート、シザー矩形
	CreateViewport();
	CreateScissor();
}

void DirectXCommon::PreDraw()
{
	//書き込むバックバッファのインデックス取得
	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

	//リソースバリア書き込み可能
	commandList_->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(swapChainResources_[backBufferIndex].Get(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));


	//描画先のRTV, DSVを設定
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap_.Heap->GetCPUDescriptorHandleForHeapStart();
	commandList_->OMSetRenderTargets(1, &rtvHandles_[backBufferIndex], false, &dsvHandle);

	//指定した色で画面をクリア
	commandList_->ClearRenderTargetView(rtvHandles_[backBufferIndex], clearColor_, 0, nullptr);
	//指定した深度で画面全体をクリア
	commandList_->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0,0, nullptr);

	//描画用のDescriptorHeapの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = {srvDescriptorHeap_.Heap.Get()};
	commandList_->SetDescriptorHeaps(1, descriptorHeaps);

	//Screen設定
	commandList_->RSSetViewports(1,&viewport_);			//ビューポート
	commandList_->RSSetScissorRects(1,&scissorRect_);	//シザー矩形
}

void DirectXCommon::PostDraw()
{
	//書き込むバックバッファのインデックス取得
	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

	//リソースバリア
	commandList_->ResourceBarrier(1,&CD3DX12_RESOURCE_BARRIER::Transition(swapChainResources_[backBufferIndex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));


	//コマンドリストの内容を確定。
	result = commandList_->Close();
	assert(SUCCEEDED(result));

	//GPUにコマンドリストの実行を行わせる
	ID3D12CommandList* commandList[] = {commandList_.Get()};
	commandQueue_->ExecuteCommandLists(1, commandList);


	//フェンス値を更新
	fenceValue_++;
	//GPUが完了時に、Fence値を指定した値に代入するようにSignalを送る
	commandQueue_->Signal(fence_.Get(), fenceValue_);

	//Fence値が指定したSignal値に達しているのか
	//GetCompletedValueの初期値はFence作成時に渡した初期値
	if(fence_->GetCompletedValue() < fenceValue_){
		//指定したSignalにたどり着いていない、待つようにイベントを指定
		fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
		//イベント待機
		WaitForSingleObject(fenceEvent_, INFINITE);
	}

	UpdateFixFPS();

	//次のフレーム用のコマンドリスト用意
	result = commandAllocator_.Get()->Reset();
	assert(SUCCEEDED(result));
	result = commandList_->Reset(commandAllocator_.Get(), nullptr);
	assert(SUCCEEDED(result));

	//GPUとOSに画面の交換
	swapChain_->Present(1,0);
}


#pragma region デバック系
bool DirectXCommon::CreateDebugLayer()
{
#ifdef _DEBUG
	//if(SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController_)))){
	//	//デバックレイヤー有効化
	//	debugController_->EnableDebugLayer();
	//	//さらにGPU側でもチェックを行う
	//	debugController_->SetEnableGPUBasedValidation(true);
	//	
	//}
	DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug_));
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
	descriptorSizeRTV_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	//RTV用のヒープでデスクリプタの数は2	RTVはShaderないで触らないので ShaderVisible = false
	rtvDescriptorHeap_.Heap = CreateDescriptorHeap(device_.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, SwapChainNum,false);
	rtvDescriptorHeap_.CPUFlags.resize(SwapChainNum);
	rtvDescriptorHeap_.CPUFlags.assign(rtvDescriptorHeap_.CPUFlags.size(), false);
	rtvDescriptorHeap_.GPUFlags.resize(SwapChainNum);
	rtvDescriptorHeap_.GPUFlags.assign(rtvDescriptorHeap_.GPUFlags.size(), false);
	return true;
}

bool DirectXCommon::CreateSRVDescriptorHeap()
{
	descriptorSizeSRV_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//SRV用のヒープでディスクリプタの数は128	SRVはShaderないで触れるものなので ShaderVisible = true
	srvDescriptorHeap_.Heap = CreateDescriptorHeap(device_.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, kSRVNumMax, true);
	srvDescriptorHeap_.CPUFlags.resize(kSRVNumMax);
	srvDescriptorHeap_.CPUFlags.assign(srvDescriptorHeap_.CPUFlags.size(), false);
	srvDescriptorHeap_.GPUFlags.resize(kSRVNumMax);
	srvDescriptorHeap_.GPUFlags.assign(srvDescriptorHeap_.GPUFlags.size(), false);
	return true;
}

bool DirectXCommon::CreateDSVDescriptorHeap()
{
	descriptorSizeDSV_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	//DSB用のヒープでディスクリプタの数は1		DSVはShaderないで降れるものではないので、ShaderVisible = false
	dsvDescriptorHeap_.Heap = CreateDescriptorHeap(device_.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);
	dsvDescriptorHeap_.CPUFlags.resize(1);
	dsvDescriptorHeap_.CPUFlags.assign(dsvDescriptorHeap_.CPUFlags.size(), false);
	dsvDescriptorHeap_.GPUFlags.resize(1);
	dsvDescriptorHeap_.GPUFlags.assign(dsvDescriptorHeap_.GPUFlags.size(), false);
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
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;	//出力結果をSRGBに変換、書き込み
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;	//2Dテクスチャとして書き込む
	
	for(int i = 0; i < SwapChainNum; i++){
		
		//作成する場所指定
		rtvHandles_[i] = GetCPUDescriptorHandle(rtvDescriptorHeap_, descriptorSizeRTV_);
		
		//生成
		device_->CreateRenderTargetView(swapChainResources_[i].Get(), &rtvDesc, rtvHandles_[i]);
	}

	return true;
}

bool DirectXCommon::CreateDSV()
{
	//DepthStencilTextureをウィンドウサイズで作成
	ID3D12Resource* depthStencilResource = CreateDepthStencilTextureResource(device_.Get(), WindowsApp::kWindowWidth_, WindowsApp::kWindowHeight_);

	//DSV作成
	{
		//DSV設定
		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
		dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	//Format　Resourceに合わせる
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;	//2dTexture

		//DSVHeapの先頭にDSVを作成
		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandleCPU = dsvDescriptorHeap_.Heap->GetCPUDescriptorHandleForHeapStart();
		device_->CreateDepthStencilView(depthStencilResource, &dsvDesc, dsvHandleCPU);
	}
	return true;
}
#pragma endregion


#pragma region フェンス
bool DirectXCommon::CreateFence()
{
	result = device_->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	assert(SUCCEEDED(result));

	//フェンスのSignalを待つイベント作成
	fenceEvent_ = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent_ != nullptr);

	return true;
}
#pragma endregion



#pragma region ViewportとScissor
bool DirectXCommon::CreateViewport()
{
	//クライアント領域のサイズと一緒に画面全体に表示
	viewport_.Width = (FLOAT)WindowsApp::kWindowWidth_;
	viewport_.Height = (FLOAT)WindowsApp::kWindowHeight_;
	viewport_.TopLeftX = 0;
	viewport_.TopLeftY = 0;
	viewport_.MinDepth = 0.0f;
	viewport_.MaxDepth = 1.0f;

	return true;
}

bool DirectXCommon::CreateScissor()
{
	//基本的にビューポートと同じ矩形を構成する
	scissorRect_.left = 0;
	scissorRect_.right = WindowsApp::kWindowWidth_;
	scissorRect_.top = 0;
	scissorRect_.bottom = WindowsApp::kWindowHeight_;

	return true;
}
#pragma endregion

void DirectXCommon::InitializeFixFPS()
{
	//現在時刻
	reference_ = std::chrono::steady_clock::now();
}
void DirectXCommon::UpdateFixFPS()
{
	//1/60Just
	const std::chrono::microseconds kMinTime(uint64_t(1000000.0f/fixedFpsValue_));
	//1/60LittleShort
	const std::chrono::microseconds kMinCheckTime(uint64_t(100000.0f/fixedFpsValue_ + 5.0f));

	//現在時刻
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
	//前回時刻との差
	std::chrono::microseconds elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - reference_);

	//1/60立っていない場合
	if(elapsed < kMinCheckTime){
		while(std::chrono::steady_clock::now() - reference_ < kMinTime){
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}

	//fps
	double frameDurationSeconds = static_cast<double>(elapsed.count())/1e6;
	fps_ = 1.0 / frameDurationSeconds;

	//現在時刻
	reference_ = std::chrono::steady_clock::now();
}