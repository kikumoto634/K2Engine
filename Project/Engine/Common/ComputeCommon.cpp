#include "ComputeCommon.h"
#include <d3dcompiler.h>
#include "DescriptorHeap.h"

#include "BufferResource.h"
#include "BufferView.h"

// 確認用配列
//std::vector<float>test(256, 0);

ComputeCommon *ComputeCommon::GetInstance()
{
	static ComputeCommon* instance = new ComputeCommon();
	instance->Initialize();
	return instance;
}

void ComputeCommon::Initialize()
{
	dxCommon = DirectXCommon::GetInstance();

	CreateRootSignature();
	CreatePipeline();

	CreateComputeDescriptorHeap();

	CreateCommand();

	CreateFence();
}


ComputeData* ComputeCommon::Excution(int instanceNum, void* data)
{
	commandList_->SetComputeRootSignature(rootSignature_.Get());
	commandList_->SetPipelineState(pipeline_.Get());
	//描画用のDescriptorHeapの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = {descriptorHeap_.Get()};
	commandList_->SetDescriptorHeaps(1, descriptorHeaps);

	D3D12_GPU_DESCRIPTOR_HANDLE handle = descriptorHeap_->GetGPUDescriptorHandleForHeapStart();
	commandList_->SetComputeRootDescriptorTable(0, handle);

	commandList_->Dispatch((UINT)instanceNum, 1,1);
	commandList_->Close();

	//GPUにコマンドリストの実行を行わせる
	ID3D12CommandList* commandList[] = {commandList_.Get()};
	commandQueue_->ExecuteCommandLists(1, commandList);

	//Fence
	fenceValue_++;
	commandQueue_->Signal(fence_.Get(), fenceValue_);
	if(fence_->GetCompletedValue() < fenceValue_){
		fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
		WaitForSingleObject(fenceEvent_, INFINITE);
	}

	commandAllocator_->Reset();
	commandList_->Reset(commandAllocator_.Get(), nullptr);

	return ((ComputeData*)data, (ComputeData*)data + instanceNum);
}


void ComputeCommon::CreateRootSignature()
{
	HRESULT result_{};

	result_ = D3DCompileFromFile(
		L"Resources/Shaders/Particle/Particle.CS.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"cs_5_1",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&computeShaderBlob_,
		nullptr
	);
	assert(SUCCEEDED(result_));


	ID3DBlob* signature = nullptr;
	result_ = D3DGetBlobPart(computeShaderBlob_->GetBufferPointer(),computeShaderBlob_->GetBufferSize(),
		D3D_BLOB_ROOT_SIGNATURE, 0, &signature);
	assert(SUCCEEDED(result_));

	result_ = dxCommon->GetDevice()->CreateRootSignature(
		0,
		signature->GetBufferPointer(), 
		signature->GetBufferSize(), 
		IID_PPV_ARGS(&rootSignature_)
	);
	assert(SUCCEEDED(result_));
}

void ComputeCommon::CreatePipeline()
{
	D3D12_COMPUTE_PIPELINE_STATE_DESC desc{};
	desc.CS.pShaderBytecode = computeShaderBlob_->GetBufferPointer();
	desc.CS.BytecodeLength = computeShaderBlob_->GetBufferSize();
	desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	desc.NodeMask = 0;
	desc.pRootSignature = rootSignature_.Get();

	HRESULT result_ = dxCommon->GetDevice()->CreateComputePipelineState(
		&desc,
		IID_PPV_ARGS(&pipeline_)
	);
	assert(SUCCEEDED(result_));
}

void ComputeCommon::CreateCommand()
{
	HRESULT result_{};

	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
	result_ = dxCommon->GetDevice()->CreateCommandQueue(
		&commandQueueDesc, 
		IID_PPV_ARGS(&commandQueue_)
	);
	assert(SUCCEEDED(result_));

	result_ = dxCommon->GetDevice()->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_COMPUTE,
		IID_PPV_ARGS(&commandAllocator_)
	);
	assert(SUCCEEDED(result_));

	result_ = dxCommon->GetDevice()->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_COMPUTE,
		commandAllocator_.Get(),
		nullptr,
		IID_PPV_ARGS(&commandList_)
	);
	assert(SUCCEEDED(result_));
}

void ComputeCommon::CreateComputeDescriptorHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC desc{};
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	desc.NodeMask = 0;
	desc.NumDescriptors = 1;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	HRESULT result_ = dxCommon->GetDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorHeap_));
	assert(SUCCEEDED(result_));
}

//void ComputeCommon::CreateResource()
//{
//	resource_ = CreateComputeBufferResource(dxCommon->GetDevice(), (sizeof(float) * (test.size()) + 0xff)&~0xff);
//}
//
//void ComputeCommon::CreateUAV()
//{
//	D3D12_UNORDERED_ACCESS_VIEW_DESC desc{};
//	CreateComputreView(desc, (UINT)test.size(), sizeof(float));
//
//	dxCommon->GetDevice()->CreateUnorderedAccessView(
//		resource_.Get(),
//		nullptr,
//		&desc,
//		descriptorHeap_->GetCPUDescriptorHandleForHeapStart()
//	);
//}

void ComputeCommon::CreateFence()
{
	HRESULT result_{};
	result_ = dxCommon->GetDevice()->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	assert(SUCCEEDED(result_));

	fenceEvent_ = CreateEvent(NULL,FALSE,FALSE,NULL);
	assert(fenceEvent_ != nullptr);
}
