#include "GPUParticleBase.h"
#include <BufferResource.h>
#include <BufferView.h>

GPUParticleBase *GPUParticleBase::Create()
{
	GPUParticleBase* instance = new GPUParticleBase();
	instance->Initialize();
	return instance;
}

void GPUParticleBase::Initialize(bool isIndexEnable)
{
	dxCommon = DirectXCommon::GetInstance();
	compute = ComputeCommon::GetInstance();

	pipeline_ = new Pipeline();
	PipelineStateInitialize();

	computeData_.resize(1);

	CreateVertex();
	CreateIndex();
	CreateMaterial();
	CreateWVP();
	CreateCompute();
}

void GPUParticleBase::Draw(Camera* camera)
{
	compute->Excution(computeData_);
	transfrom_.translate = computeData_[0].position;

	Matrix4x4 worldViewProjectionMatrix = transfrom_.GetWorldMatrix() * camera->GetViewProjectionMatrix();
	*wvpData_ = worldViewProjectionMatrix;


	//ルートシグネチャ設定 PSOに設定しいているが別途設定が必要
	dxCommon->GetCommandList()->SetGraphicsRootSignature(pipeline_->GetRootSignature());
	dxCommon->GetCommandList()->SetPipelineState(pipeline_->GetGraphicsPipelineState());	//PSO設定
	dxCommon->GetCommandList()->IASetVertexBuffers(0,1,&vertexBufferView_);		//VBV設定
	dxCommon->GetCommandList()->IASetIndexBuffer(&indexBufferView_);		//IBV設定

	//形状設定、PSOに設定しているのとは別
	dxCommon->GetCommandList()->IASetPrimitiveTopology(commandPrimitiveTopology_);

	//行列のwvpBufferの場所を設定 ※RootParameter[1]に対してCBVの設定
	dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, wvpResource_->GetGPUVirtualAddress());
	//マテリアルのconstBufferの場所を設定
	dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, constResource_->GetGPUVirtualAddress());

	//描画
	dxCommon->GetCommandList()->DrawIndexedInstanced(indexNum_,1,0,0,0);
}



void GPUParticleBase::PipelineStateInitialize()
{
	//デスクリプタレンジ(SRV, CBVなどの情報をこれにまとめる)
	//例 :														Shaderでは
	//range[0]					range[1]						ConstBuffer<..>gMaterial0 : register(b0)
	//BaseRegister = 3;			BaseRegister = 0;				ConstBuffer<..>gMaterial1 : register(b1)
	//numDescriptor = 2;		NumDescritor = 3;				ConstBuffer<..>gMaterial2 : register(b2)
	//Type = SRV;				Type = CBV;						Texture2D<..> gTexture0 : register(t3)
	//															Texture2D<..> gTexture1 : register(t4)

	//現在
	//DescriptorHeap (Index / 対象名)
	// 0 / ImGui
	// 1~x / SRV(ParticleMatrix)
	// 2~x / SRV(Texture)
	// 3+x / CSV(VERTEX)
	// 3+x / CSV(VERTEX)
	//ルートパラメータ設定
	rootParameters_.resize(2);
	//VS(行列)
	rootParameters_[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		//CBV
	rootParameters_[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;	//VertexShaderで使用
	rootParameters_[0].Descriptor.ShaderRegister = 0;	//レジスタ番号 b0
	//PS(色)
	rootParameters_[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		//CBV
	rootParameters_[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	//PixelShader使用
	rootParameters_[1].Descriptor.ShaderRegister = 0;	//レジスタ番号 b0	
	
	//インプットレイアウト設定(頂点データでシェーダ内に送るデータたちのセマンティック名)
	inputElementDesc_.resize(1);
	inputElementDesc_[0].SemanticName = "POSITION";							//頂点シェーダーのセマンティック名
	inputElementDesc_[0].SemanticIndex = 0;									//セマンティック番号
	inputElementDesc_[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;			//float4 型
	inputElementDesc_[0].InputSlot = 0;
	inputElementDesc_[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDesc_[0].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	inputElementDesc_[0].InstanceDataStepRate = 0;


	//生成
	std::wstring vs = WindowsApp::ConvertString(VSPath_);
	std::wstring ps = WindowsApp::ConvertString(PSPath_);

	pipeline_->DepthStencilSet();
	pipeline_->Create(
		vs,
		ps,
		rootParameters_,
		{},
		inputElementDesc_,
		fillMode_,
		pipelinePrimitiveTopology_,
		BlendSetting::kBlendModeNormal,
		D3D12_CULL_MODE_NONE
	);
}

void GPUParticleBase::CreateVertex()
{
	//リソース
	vertexResource_ = CreateBufferResource(dxCommon->GetDevice(), sizeof(Vector4)*vertNum_);
	//ビュー
	CreateBufferView(vertexBufferView_, vertexResource_.Get(), sizeof(Vector4)*vertNum_, sizeof(Vector4));
	//頂点リソースにデータを書き込む
	//書き込むためのアドレス取得
	vertexResource_->Map(0,nullptr,reinterpret_cast<void**>(&vertData_));

	vertData_[0] = {-0.5f,-0.5f,-0.5f, 1.0f};
	vertData_[1] = { 0.5f,-0.5f,-0.5f, 1.0f};
	vertData_[2] = { 0.5f, 0.5f,-0.5f, 1.0f};
	vertData_[3] = {-0.5f, 0.5f,-0.5f, 1.0f};

	vertData_[4] = {-0.5f,-0.5f, 0.5f, 1.0f};
	vertData_[5] = { 0.5f,-0.5f, 0.5f, 1.0f};
	vertData_[6] = { 0.5f, 0.5f, 0.5f, 1.0f};
	vertData_[7] = {-0.5f, 0.5f, 0.5f, 1.0f};
}

void GPUParticleBase::CreateIndex()
{
	indexResource_ = CreateBufferResource(dxCommon->GetDevice(), sizeof(uint32_t)*indexNum_);

	CreateBufferView(indexBufferView_, indexResource_.Get(), sizeof(uint32_t)*indexNum_);

	//インデックスリソースにデータを書き込む
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));

	indexData_[0] = 3;	indexData_[1] = 2;	indexData_[2] = 1;
	indexData_[3] = 0;	indexData_[4] = 3;	indexData_[5] = 1;
	
	indexData_[6] = 5;	indexData_[7] = 6;	indexData_[8] = 7;
	indexData_[9] = 5;	indexData_[10] = 7;	indexData_[11] = 4;
	
	indexData_[12] = 7;	indexData_[13] = 3;	indexData_[14] = 0;
	indexData_[15] = 0;	indexData_[16] = 4;	indexData_[17] = 7;
	
	indexData_[18] = 6;	indexData_[19] = 5;	indexData_[20] = 1;
	indexData_[21] = 1;	indexData_[22] = 2;	indexData_[23] = 6;
	
	indexData_[24] = 6;	indexData_[25] = 2;	indexData_[26] = 3;
	indexData_[27] = 3;	indexData_[28] = 7;	indexData_[29] = 6;

	indexData_[30] = 0;	indexData_[31] = 1;	indexData_[32] = 5;
	indexData_[33] = 5;	indexData_[34] = 4;	indexData_[35] = 0;
}

void GPUParticleBase::CreateMaterial()
{
	constResource_ = CreateBufferResource(dxCommon->GetDevice(), sizeof(Vector4));

	constResource_->Map(0,nullptr,reinterpret_cast<void**>(&materialData_));
	*materialData_ = color_;
}

void GPUParticleBase::CreateWVP()
{
	wvpResource_ = CreateBufferResource(dxCommon->GetDevice(), sizeof(Matrix4x4));

	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
	
	Matrix4x4 worldMatrix = worldMatrix.MakeIdentityMatrix();
	*wvpData_ = worldMatrix;
}

void GPUParticleBase::CreateCompute()
{
	//リソース
	computeResource_ = CreateComputeBufferResource(
		dxCommon->GetDevice(),
		(sizeof(Sample) * (computeData_.size()) + 0xff)&~0xff
	);

	//ビュー
	D3D12_UNORDERED_ACCESS_VIEW_DESC desc{};
	CreateComputreView(desc, (UINT)computeData_.size(), sizeof(Sample));

	dxCommon->GetDevice()->CreateUnorderedAccessView(
		computeResource_.Get(),
		nullptr,
		&desc,
		compute->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart()
	);

	compute->Map(computeResource_.Get());
}