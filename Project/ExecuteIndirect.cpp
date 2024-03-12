#include "ExecuteIndirect.h"
#include "BufferResource.h"
#include "BufferView.h"
#include "DescriptorHeap.h"

#include <d3dx12.h>
#include <random>

ExecuteIndirect::~ExecuteIndirect()
{
	delete pipeline_;
}

void ExecuteIndirect::Initialize()
{
	dxCommon = DirectXCommon::GetInstance();


	CreatePipeline();
	CreateVertex();
	CreateMaterial();
	CreateWVP();

	std::random_device seedGenerator_;
	std::mt19937 randomEngine(seedGenerator_());
	std::uniform_real_distribution<float> distValue(-10.f,10.f);
	for(int i = 0; i < (int)kInstanceNum; i++){
		Vector3 temp = {distValue(randomEngine), distValue(randomEngine), distValue(randomEngine)};
		//Vector3 temp = {};

		transform.push_back({temp, {0,0,0}, {1,1,1}});
	}


	//コマンドシグネチャ
	D3D12_INDIRECT_ARGUMENT_DESC argumentDescs[2] = {};
	//色
	argumentDescs[0].Type = D3D12_INDIRECT_ARGUMENT_TYPE_CONSTANT_BUFFER_VIEW;
	argumentDescs[0].ConstantBufferView.RootParameterIndex = 0;
	//描画コマンド
	argumentDescs[1].Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW;

	D3D12_COMMAND_SIGNATURE_DESC commandSignatureDesc = {};
	commandSignatureDesc.pArgumentDescs = argumentDescs;
	commandSignatureDesc.NumArgumentDescs = _countof(argumentDescs);
	commandSignatureDesc.ByteStride = sizeof(IndirectCommand);

	dxCommon->GetDevice()->CreateCommandSignature(&commandSignatureDesc, pipeline_->GetRootSignature(),IID_PPV_ARGS(&commandSignature_));

	
	//コマンドバッファ(サイズは使用するリソース sizeが謎?
	commandResource_ = CreateBufferUploadResource(dxCommon->GetDevice(), (sizeof(IndirectCommand)) * kCommandNum);

	//コマンドバッファのマップ
	IndirectCommand* mapIndirectCommamdData = nullptr;
	commandResource_->Map(0,nullptr, reinterpret_cast<void**>(&mapIndirectCommamdData));

	//転送する各リソースのメモリサイズ
	//マテリアルのサイズ
	D3D12_GPU_VIRTUAL_ADDRESS materialGpuAddress = materialResource_->GetGPUVirtualAddress();

	for(UINT i = 0; i < kCommandNum; ++i){
		
		//色
		mapIndirectCommamdData[i].material = materialGpuAddress;

		mapIndirectCommamdData[i].drawArguments.VertexCountPerInstance = vertNum;
		mapIndirectCommamdData[i].drawArguments.InstanceCount = kInstanceNum;
		mapIndirectCommamdData[i].drawArguments.StartVertexLocation = 0;
		mapIndirectCommamdData[i].drawArguments.StartInstanceLocation = 0;

		//色のGPUアドレスをサイズ分更新
		materialGpuAddress += sizeof(Vector4);
	}

	commandResource_->Unmap(0,nullptr);
}

void ExecuteIndirect::Update()
{
}

void ExecuteIndirect::Draw(Camera* camera)
{
	#pragma omp parallel for
	for(int i = 0; i < (int)kInstanceNum; i++){
		wvpData_[i] = transform[i].GetWorldMatrix() * camera->GetViewProjectionMatrix();
	}

	//ルートシグネチャ設定 PSOに設定しいているが別途設定が必要
	dxCommon->GetCommandList()->SetGraphicsRootSignature(pipeline_->GetRootSignature());
	dxCommon->GetCommandList()->SetPipelineState(pipeline_->GetGraphicsPipelineState());	//PSO設定
	dxCommon->GetCommandList()->IASetVertexBuffers(0,1,&vertexBufferView_);		//VBV設定

	//形状設定、PSOに設定しているのとは別
	dxCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	//色
	dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	//行列
	dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(1,wvpInstancingGPU_);

	//描画
	//dxCommon->GetCommandList()->DrawInstanced(3,1,0,0);
	dxCommon->GetCommandList()->ExecuteIndirect(
		commandSignature_.Get(),
		kCommandNum,
		commandResource_.Get(),
		0,
		nullptr,
		0
	);
}


void ExecuteIndirect::CreatePipeline()
{
	pipeline_ = new Pipeline();

	D3D12_DESCRIPTOR_RANGE SRVDescriptorRange[1] = {};
	//行列
	SRVDescriptorRange[0].BaseShaderRegister = 0;
	SRVDescriptorRange[0].NumDescriptors = 1;
	SRVDescriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	SRVDescriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//ルートパラメータ
	rootParameters_.resize(2);
	//色
	rootParameters_[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters_[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters_[0].Descriptor.ShaderRegister = 0;
	//行列
	rootParameters_[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters_[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters_[1].DescriptorTable.pDescriptorRanges = &SRVDescriptorRange[0];
	rootParameters_[1].DescriptorTable.NumDescriptorRanges = 1;

	//インプットレイアウト
	inputElementDesc_.resize(1);
	inputElementDesc_[0].SemanticName = "POSITION";							//頂点シェーダーのセマンティック名
	inputElementDesc_[0].SemanticIndex = 0;									//セマンティック番号
	inputElementDesc_[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;			//float4 型
	inputElementDesc_[0].InputSlot = 0;
	inputElementDesc_[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDesc_[0].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	inputElementDesc_[0].InstanceDataStepRate = 0;

	std::wstring vs = WindowsApp::ConvertString("Execute.VS.hlsl");
	std::wstring ps = WindowsApp::ConvertString("Execute.PS.hlsl");

	pipeline_->DepthStencilSet();
	pipeline_->Create(
		vs,
		ps,
		rootParameters_,
		{},
		inputElementDesc_,
		D3D12_FILL_MODE_SOLID,
		//D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT
	);
}

void ExecuteIndirect::CreateVertex()
{
	//リソース
	vertexResource_ = CreateBufferUploadResource(dxCommon->GetDevice(), sizeof(Vector4)*vertNum);
	//ビュー
	CreateBufferView(vertexBufferView_, vertexResource_.Get(), sizeof(Vector4)*vertNum, sizeof(Vector4));

	//頂点リソースにデータを書き込む
	//書き込むためのアドレス取得
	vertexResource_->Map(0,nullptr,reinterpret_cast<void**>(&vertData_));
	
	vertData_[0] = {0.0f, 0.0f, 0.0f, 1.0f};
	
	//vertData_[0] = {-0.1f, -0.1f, 0.0f, 1.0f};
	//vertData_[1] = {+0.0f, +0.1f, 0.0f, 1.0f};
	//vertData_[2] = {+0.1f, -0.1f, 0.0f, 1.0f};
}

void ExecuteIndirect::CreateMaterial()
{
	//リソース
	materialResource_ = CreateBufferUploadResource(dxCommon->GetDevice(), sizeof(Vector4));
	//頂点リソースにデータを書き込む
	//書き込むためのアドレス取得
	materialResource_->Map(0,nullptr,reinterpret_cast<void**>(&materialData_));

	*materialData_ = Vector4(1,0,0,1);
}

void ExecuteIndirect::CreateWVP()
{
	//リソース
	wvpResource_ = CreateBufferUploadResource(dxCommon->GetDevice(), sizeof(Matrix4x4)*kInstanceNum);
	//頂点リソースにデータを書き込む
	//書き込むためのアドレス取得
	wvpResource_->Map(0,nullptr,reinterpret_cast<void**>(&wvpData_));

	for(int i = 0; i < (int)kInstanceNum; i++){
		wvpData_[i] = MakeIdentityMatrix();
	}
	D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDeasc{};
	instancingSrvDeasc.Format = DXGI_FORMAT_UNKNOWN;
	instancingSrvDeasc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancingSrvDeasc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	instancingSrvDeasc.Buffer.FirstElement = 0;
	instancingSrvDeasc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	instancingSrvDeasc.Buffer.NumElements = kInstanceNum;
	instancingSrvDeasc.Buffer.StructureByteStride = sizeof(Matrix4x4);

	wvpInstancingCPU_ = GetCPUDescriptorHandle(dxCommon->GetSRVDescriptorHeap(), dxCommon->GetDescriptorSizeSRV());
	wvpInstancingGPU_ = GetGPUDescriptorHandle(dxCommon->GetSRVDescriptorHeap(), dxCommon->GetDescriptorSizeSRV());
	dxCommon->GetDevice()->CreateShaderResourceView(wvpResource_.Get(), &instancingSrvDeasc, wvpInstancingCPU_);
}
