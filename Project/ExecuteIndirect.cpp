#include "ExecuteIndirect.h"
#include "BufferResource.h"
#include "BufferView.h"

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


	//コマンドシグネチャ
	D3D12_INDIRECT_ARGUMENT_DESC argumentDescs[1] = {};
	argumentDescs[0].Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW;

	D3D12_COMMAND_SIGNATURE_DESC commandSignatureDesc = {};
	commandSignatureDesc.pArgumentDescs = argumentDescs;
	commandSignatureDesc.NumArgumentDescs = _countof(argumentDescs);
	commandSignatureDesc.ByteStride = sizeof(IndirectCommand);

	dxCommon->GetDevice()->CreateCommandSignature(&commandSignatureDesc, nullptr,IID_PPV_ARGS(&commandSignature_));

	
	//コマンドバッファ(サイズは使用するリソース 頂点(Vector4*3))
	commandBuffer_ = CreateBufferResource(dxCommon->GetDevice(), (sizeof(Vector4)*vertNum) * kCommandNum);

	//コマンドバッファのマップ
	IndirectCommand* mapIndirectCommamdData = nullptr;
	commandBuffer_->Map(0,nullptr, reinterpret_cast<void**>(&mapIndirectCommamdData));

	for(UINT i = 0; i < kCommandNum; ++i){
		mapIndirectCommamdData[i].drawArguments.VertexCountPerInstance = vertNum;
		mapIndirectCommamdData[i].drawArguments.InstanceCount = kInstanceNum;
		mapIndirectCommamdData[i].drawArguments.StartVertexLocation = 0;
		mapIndirectCommamdData[i].drawArguments.StartInstanceLocation = 0;
	}

	commandBuffer_->Unmap(0,nullptr);
}

void ExecuteIndirect::Update()
{
}

void ExecuteIndirect::Draw(Camera* camera)
{
	//ルートシグネチャ設定 PSOに設定しいているが別途設定が必要
	dxCommon->GetCommandList()->SetGraphicsRootSignature(pipeline_->GetRootSignature());
	dxCommon->GetCommandList()->SetPipelineState(pipeline_->GetGraphicsPipelineState());	//PSO設定
	dxCommon->GetCommandList()->IASetVertexBuffers(0,1,&vertexBufferView_);		//VBV設定

	//形状設定、PSOに設定しているのとは別
	dxCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	//描画
	//dxCommon->GetCommandList()->DrawInstanced(3,1,0,0);
	dxCommon->GetCommandList()->ExecuteIndirect(
		commandSignature_.Get(),
		kCommandNum,
		commandBuffer_.Get(),
		0,
		nullptr,
		0
	);
}


void ExecuteIndirect::CreatePipeline()
{
	pipeline_ = new Pipeline();

	//ルートパラメータ
	rootParameters_.resize(0);

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
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT
	);
}

void ExecuteIndirect::CreateVertex()
{
	//リソース
	vertexResource_ = CreateBufferResource(dxCommon->GetDevice(), sizeof(Vector4)*vertNum);
	//ビュー
	CreateBufferView(vertexBufferView_, vertexResource_.Get(), sizeof(Vector4)*vertNum, sizeof(Vector4));

	//頂点リソースにデータを書き込む
	//書き込むためのアドレス取得
	vertexResource_->Map(0,nullptr,reinterpret_cast<void**>(&vertData_));


	std::random_device seedGenerator_;
	std::mt19937 randomEngine(seedGenerator_());
	std::uniform_real_distribution<float> distValue(-0.5f,0.5f);
	//vertData_[0] = {distValue(randomEngine), distValue(randomEngine), 0.0f, 1.0f};
	vertData_[0] = {-0.8f, -0.1f, 0.0f, 1.0f};
	//vertData_[1] = {+0.0f, +0.1f, 0.0f, 1.0f};
	//vertData_[2] = {+0.1f, -0.1f, 0.0f, 1.0f};
}

void ExecuteIndirect::CreateMaterial()
{
	//リソース
	materialResource_ = CreateBufferResource(dxCommon->GetDevice(), sizeof(Vector4));
	//頂点リソースにデータを書き込む
	//書き込むためのアドレス取得
	vertexResource_->Map(0,nullptr,reinterpret_cast<void**>(&materialData_));

	*materialData_ = Vector4(1,0,0,1);
}
