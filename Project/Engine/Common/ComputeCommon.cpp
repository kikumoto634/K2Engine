#include "ComputeCommon.h"
#include <d3dcompiler.h>

#include "DescriptorHeap.h"

#pragma comment(lib, "dxcompiler.lib")

// 確認用配列
std::vector<float>test(256, 0);

void ComputeCommon::Initialize()
{
	dxCommon = DirectXCommon::GetInstance();

	CreateRootSignature();
	CreatePipeline();

	CreateComputeDescriptorHeap();
	CreateResource();
	CreateUAV();

	CreateCommand();

	CreateFence();

	D3D12_RANGE range{0,1};
	HRESULT result_ = resource_->Map(0,&range,&data);
}

void ComputeCommon::Excution()
{
	commandAllocator_->Reset();
	commandList_->Reset(commandAllocator_.Get(), nullptr);

	commandList_->SetComputeRootSignature(rootSignature_.Get());
	commandList_->SetPipelineState(pipeline_.Get());
	commandList_->SetDescriptorHeaps(1, &descriptorHeap_);

	auto handle = descriptorHeap_->GetGPUDescriptorHandleForHeapStart();
	commandList_->SetComputeRootDescriptorTable(0, handle);

	commandList_->Dispatch(test.size(), 1,1);
	commandList_->Close();

	ID3D12CommandList* com[] = {
		commandList_.Get()
	};

	dxCommon->GetCommandQueue()->ExecuteCommandLists(1, com);

	//Fence
	fenceValue_++;
	dxCommon->GetCommandQueue()->Signal(fence_.Get(), fenceValue_);
	if(fence_->GetCompletedValue() < fenceValue_){
		fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
		WaitForSingleObject(fenceEvent_, INFINITE);
	}

	test.assign((float*)data, (float*)data + test.size());
}


void ComputeCommon::CreateRootSignature()
{
	HRESULT result_{};
	result_ = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	assert(SUCCEEDED(result_));
	result_ = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
	assert(SUCCEEDED(result_));

	//現時点でのincludeはしないが、includeに対応するための設定を行う
	result_ = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(result_));

	computeShaderBlob_ = Pipeline::CompileShader(
		L"Resources/Shaders/Particle/Particle.CS.hlsl",
		L"cs_6_0",
		dxcUtils_.Get(),
		dxcCompiler_.Get(),
		includeHandler_.Get()
	);

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
	D3D12_COMPUTE_PIPELINE_STATE_DESC desc;
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
	descriptorHeap_ = CreateDescriptorHeap(
		dxCommon->GetDevice(),
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		1,
		true
	);
}

void ComputeCommon::CreateResource()
{
	D3D12_HEAP_PROPERTIES prop{};
	prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	prop.CreationNodeMask = 1;
	prop.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	prop.Type = D3D12_HEAP_TYPE_CUSTOM;
	prop.VisibleNodeMask = 1;
	
	D3D12_RESOURCE_DESC desc{};
	desc.Alignment = 0;
	desc.DepthOrArraySize = 1;
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.Height = 1;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	desc.MipLevels = 1;
	desc.SampleDesc = {1,0};
	desc.Width = (sizeof(float) * (test.size()) + 0xff)&~0xff;

	HRESULT result_ = dxCommon->GetDevice()->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		nullptr,
		IID_PPV_ARGS(&resource_)
	);
	assert(SUCCEEDED(result_));
}

void ComputeCommon::CreateUAV()
{
	D3D12_UNORDERED_ACCESS_VIEW_DESC desc{};
	desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.Buffer.NumElements = test.size();
	desc.Buffer.StructureByteStride = sizeof(float);

	dxCommon->GetDevice()->CreateUnorderedAccessView(
		resource_.Get(),
		nullptr,
		&desc,
		descriptorHeap_->GetCPUDescriptorHandleForHeapStart()
	);
}

void ComputeCommon::CreateFence()
{
	HRESULT result_{};
	result_ = dxCommon->GetDevice()->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	assert(SUCCEEDED(result_));

	fenceEvent_ = CreateEvent(NULL,FALSE,FALSE,NULL);
	assert(fenceEvent_ != nullptr);
}
