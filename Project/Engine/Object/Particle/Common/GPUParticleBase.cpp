#include "GPUParticleBase.h"
#include <BufferResource.h>
#include <BufferView.h>
#include <DescriptorHeap.h>
#include <random>

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

	CreateVertex();
	CreateMaterial();
	CreateWVP();
	CreateCompute();

	for(int i = 0; i < kNumMaxInstance; i++){
		Transform temp1;
		transfrom_.push_back(temp1);
	}
}

void GPUParticleBase::Draw(Camera* camera)
{
	compute->Excution(kNumMaxInstance, reinterpret_cast<void**>(&computeData_));

	int numInstance = 0;	//描画すべきインスタンス
	for(list<Transform>::iterator particleIterator = transfrom_.begin(); particleIterator != transfrom_.end();){
		if(numInstance < kNumMaxInstance){
			
			particleIterator->translate = computeData_[numInstance].position;

			Matrix4x4 worldViewProjectionMatrix = particleIterator->GetWorldMatrix() * camera->GetViewProjectionMatrix();
			wvpData_[numInstance] = worldViewProjectionMatrix;

			++numInstance;
		}
		++particleIterator;
	}


	//ルートシグネチャ設定 PSOに設定しいているが別途設定が必要
	dxCommon->GetCommandList()->SetGraphicsRootSignature(pipeline_->GetRootSignature());
	dxCommon->GetCommandList()->SetPipelineState(pipeline_->GetGraphicsPipelineState());	//PSO設定
	dxCommon->GetCommandList()->IASetVertexBuffers(0,1,&vertexBufferView_);		//VBV設定

	//形状設定、PSOに設定しているのとは別
	dxCommon->GetCommandList()->IASetPrimitiveTopology(commandPrimitiveTopology_);

	//行列のwvpBufferの場所を設定 ※RootParameter[1]に対してCBVの設定
	dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(0, instancingSrvHandleGPU_);
	//マテリアルのconstBufferの場所を設定
	dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, constResource_->GetGPUVirtualAddress());

	//描画
	dxCommon->GetCommandList()->DrawInstanced(vertNum_,numInstance,0,0);
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
	
	//SRV(InstancedID)
	D3D12_DESCRIPTOR_RANGE descriptorRangeForInstancing[1] = {};
	descriptorRangeForInstancing[0].BaseShaderRegister = 0;
	descriptorRangeForInstancing[0].NumDescriptors = 1;
	descriptorRangeForInstancing[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRangeForInstancing[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//ルートパラメータ設定
	rootParameters_.resize(2);
	//VS(行列)
	rootParameters_[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;		//CBV
	rootParameters_[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;	//VertexShaderで使用
	rootParameters_[0].DescriptorTable.pDescriptorRanges = descriptorRangeForInstancing;	//レジスタ番号 b0
	rootParameters_[0].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForInstancing);
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

	vertData_[0] = {0.0f,0.0f,0.0f, 1.0f};
}

void GPUParticleBase::CreateMaterial()
{
	constResource_ = CreateBufferResource(dxCommon->GetDevice(), sizeof(Vector4));

	constResource_->Map(0,nullptr,reinterpret_cast<void**>(&materialData_));
	*materialData_ = color_;
}

void GPUParticleBase::CreateWVP()
{
	wvpResource_ = CreateBufferResource(dxCommon->GetDevice(), sizeof(Matrix4x4)*kNumMaxInstance);

	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
	
	for(int i = 0; i < kNumMaxInstance; i++){
		wvpData_[i] = MakeIdentityMatrix();
	}

	//SRV作成
	D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc{};
	instancingSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	instancingSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancingSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	instancingSrvDesc.Buffer.FirstElement = 0;
	instancingSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	instancingSrvDesc.Buffer.NumElements = kNumMaxInstance;
	instancingSrvDesc.Buffer.StructureByteStride = sizeof(Matrix4x4);

	instancingSrvHandleCPU_ = GetCPUDescriptorHandle(dxCommon->GetSRVDescriptorHeap(), dxCommon->GetDescriptorSizeSRV());
	instancingSrvHandleGPU_ = GetGPUDescriptorHandle(dxCommon->GetSRVDescriptorHeap(), dxCommon->GetDescriptorSizeSRV());
	dxCommon->GetDevice()->CreateShaderResourceView(wvpResource_.Get(), &instancingSrvDesc, instancingSrvHandleCPU_);
}

void GPUParticleBase::CreateCompute()
{
	//リソース
	computeResource_ = CreateComputeBufferResource(
		dxCommon->GetDevice(),
		(sizeof(Sample) * kNumMaxInstance + 0xff)&~0xff
	);

	//ビュー
	D3D12_UNORDERED_ACCESS_VIEW_DESC desc{};
	CreateComputreView(desc, (UINT)kNumMaxInstance, sizeof(Sample));

	dxCommon->GetDevice()->CreateUnorderedAccessView(
		computeResource_.Get(),
		nullptr,
		&desc,
		compute->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart()
	);

	computeResource_->Map(0,nullptr, reinterpret_cast<void**>(&computeData_));

	//乱数生成器
	std::random_device seedGenerator_;
	std::mt19937 randomEngine(seedGenerator_());
	for(int i = 0; i < kNumMaxInstance; i++){
		std::uniform_real_distribution<float> distValue(-8,8);

		computeData_[i].position = {0, distValue(randomEngine), distValue(randomEngine)};
		computeData_[i].velocity = 1.5f;
		computeData_[i].time = 0.0f;
	}
}