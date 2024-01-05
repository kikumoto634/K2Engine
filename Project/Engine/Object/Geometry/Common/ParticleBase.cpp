#include "ParticleBase.h"

#include "SpriteLoader.h"

#include "BufferResource.h"
#include "BufferView.h"

#include "GlobalVariables.h"
#include "DescriptorHeap.h"

#include "FollowCamera.h"

void ParticleBase::Initialize(bool isIndexEnable)
{
	dxCommon = DirectXCommon::GetInstance();

	isIndexDataEnable_ = isIndexEnable;
	texture_ = SpriteLoader::SearchTexture(texturePath_);

	//パイプライン
	pipeline_ = new Pipeline();
	PipelineStateInitialize();	//パイプライン初期化

	//リソース
	CreateVertex();
	CreateIndex();
	CreateWVP();

	billboardMatrix_ = MakeIdentityMatrix();
}

void ParticleBase::Draw(Camera* camera)
{
	int numInstance = 0;	//描画すべきインスタンス
	for(int i = 0; i < kNumMaxInstance_; i++){
		if(particles_[i].lifeTime <= particles_[i].currentTime) continue;

		Matrix4x4 worldViewProjectionMatrix = MakeIdentityMatrix();
		//ビルボードなし
		if(billboardTypeEnable[0]){
			worldViewProjectionMatrix = particles_[i].transform.GetWorldMatrix() * camera->GetViewProjectionMatrix();
		}
		//全方位ビルボード
		else if(billboardTypeEnable[1]){
			billboardMatrix_ = camera->GetWorldMatrix();
			billboardMatrix_.m[3][0] = 0.0f;
			billboardMatrix_.m[3][1] = 0.0f;
			billboardMatrix_.m[3][2] = 0.0f;
			
			worldViewProjectionMatrix = particles_[i].transform.GetWorldMatrix() * billboardMatrix_ * camera->GetViewProjectionMatrix();
		}
		//Y軸ビルボード
		else if(billboardTypeEnable[2]){
			Matrix4x4 cameraMat = MakeAffineMatrix(camera->scale, {0,camera->rotation.y,camera->rotation.z}, camera->translate);

			billboardMatrix_ = cameraMat;
			billboardMatrix_.m[3][0] = 0.0f;
			billboardMatrix_.m[3][1] = 0.0f;
			billboardMatrix_.m[3][2] = 0.0f;
			
			worldViewProjectionMatrix = particles_[i].transform.GetWorldMatrix() * billboardMatrix_ * camera->GetViewProjectionMatrix();
		}

		particleData_[numInstance].WVP = worldViewProjectionMatrix;
		particleData_[numInstance].World = worldViewProjectionMatrix;

		float alpha = 1.0f - (particles_[i].currentTime / particles_[i].lifeTime);
		particleData_[numInstance].color = particles_[i].color;
		particleData_[numInstance].color.w = alpha;

		numInstance++;
	}

	//ルートシグネチャ設定 PSOに設定しいているが別途設定が必要
	dxCommon->GetCommandList()->SetGraphicsRootSignature(pipeline_->GetRootSignature());
	dxCommon->GetCommandList()->SetPipelineState(pipeline_->GetGraphicsPipelineState());	//PSO設定
	dxCommon->GetCommandList()->IASetVertexBuffers(0,1,&vertexBufferView_);		//VBV設定
	if(isIndexDataEnable_)dxCommon->GetCommandList()->IASetIndexBuffer(&indexBufferView_);		//IBV設定

	//形状設定、PSOに設定しているのとは別
	dxCommon->GetCommandList()->IASetPrimitiveTopology(commandPrimitiveTopology_);


	//SRV(テクスチャ)のDescriptorTableの先頭を設定 2はRootParamterのインデックスRootParamter[2]
	dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(0, texture_.srvHandleGPU_);
	//行列のwvpBufferの場所を設定 ※RootParameter[1]に対してCBVの設定
	dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(1,instancingSrvHandleGPU_);

	//描画
	isIndexDataEnable_ ? 
		dxCommon->GetCommandList()->DrawIndexedInstanced(indexNum_,numInstance,0,0,0) : 
		dxCommon->GetCommandList()->DrawInstanced(vertNum_,numInstance,0,0);
}


void ParticleBase::PipelineStateInitialize()
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
	// 4+x / CSV(PIXEL)

	//SRV(Texture)
	D3D12_DESCRIPTOR_RANGE SRVDescriptorRange[1] = {};
	SRVDescriptorRange[0].BaseShaderRegister = 0;	//0から開始
	SRVDescriptorRange[0].NumDescriptors = 1;	//数は1
	SRVDescriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;	//SRVを使用
	SRVDescriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;		//Offsetを自動計算

	//SRV(InstancedID)
	D3D12_DESCRIPTOR_RANGE descriptorRangeForInstancing[1] = {};
	descriptorRangeForInstancing[0].BaseShaderRegister = 0;
	descriptorRangeForInstancing[0].NumDescriptors = 1;
	descriptorRangeForInstancing[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRangeForInstancing[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//ルートパラメータ設定
	rootParameters_.resize(2);
	//SRV(テクスチャ		シェーダでは各ピクセルのことをいう)
	rootParameters_[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	//DescriptorTableに使用
	rootParameters_[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	//PixelShaderで使用
	rootParameters_[0].DescriptorTable.pDescriptorRanges = SRVDescriptorRange;	//tableの中身の配列を指定
	rootParameters_[0].DescriptorTable.NumDescriptorRanges = _countof(SRVDescriptorRange);	//Tableで利用する数
	//VS(行列)
	rootParameters_[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;		//CBV
	rootParameters_[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;	//VertexShaderで使用
	rootParameters_[1].DescriptorTable.pDescriptorRanges = descriptorRangeForInstancing;
	rootParameters_[1].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForInstancing);

	//Sampler設定(シェーダーのPS SamplerState　シェーダでは画像のことをいう)
	staticSamplers_.resize(1);
	staticSamplers_[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;	//バイリニアフィルタ・
	staticSamplers_[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	//0 ~ 1の範囲外をリピート
	staticSamplers_[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	
	staticSamplers_[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers_[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSamplers_[0].MaxLOD = D3D12_FLOAT32_MAX;	//ありったけのMipMapを使用
	staticSamplers_[0].ShaderRegister = 0;	//レジスタ番号0
	staticSamplers_[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	//PixelShaderで使用

	
	//インプットレイアウト設定(頂点データでシェーダ内に送るデータたちのセマンティック名)
	inputElementDesc_.resize(2);
	inputElementDesc_[0].SemanticName = "POSITION";							//頂点シェーダーのセマンティック名
	inputElementDesc_[0].SemanticIndex = 0;									//セマンティック番号
	inputElementDesc_[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;			//float4 型
	inputElementDesc_[0].InputSlot = 0;
	inputElementDesc_[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDesc_[0].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	inputElementDesc_[0].InstanceDataStepRate = 0;

	inputElementDesc_[1].SemanticName = "TEXCOORD";							//頂点シェーダーのセマンティック名
	inputElementDesc_[1].SemanticIndex = 0;									//セマンティック番号
	inputElementDesc_[1].Format = DXGI_FORMAT_R32G32_FLOAT;					//float4 型
	inputElementDesc_[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	//生成
	std::wstring vs = WindowsApp::ConvertString(VSPath_);
	std::wstring ps = WindowsApp::ConvertString(PSPath_);

	pipeline_->DepthStencilSet(true, D3D12_DEPTH_WRITE_MASK_ZERO);
	pipeline_->Create(
		vs,
		ps,
		rootParameters_,
		staticSamplers_,
		inputElementDesc_,
		fillMode_,
		pipelinePrimitiveTopology_,
		blendMode_
	);
}

void ParticleBase::CreateVertex()
{
	//リソース
	vertexResource_ = CreateBufferResource(dxCommon->GetDevice(), sizeof(VertexData)*vertNum_);
	//ビュー
	CreateBufferView(vertexBufferView_, vertexResource_.Get(), sizeof(VertexData)*vertNum_, sizeof(VertexData));

	//頂点リソースにデータを書き込む
	//書き込むためのアドレス取得
	vertexResource_->Map(0,nullptr,reinterpret_cast<void**>(&vertData_));
}

void ParticleBase::CreateIndex()
{
	if(!isIndexDataEnable_) return ;

	indexResource_ = CreateBufferResource(dxCommon->GetDevice(), sizeof(uint32_t)*indexNum_);

	CreateBufferView(indexBufferView_, indexResource_.Get(), sizeof(uint32_t)*indexNum_);

	//インデックスリソースにデータを書き込む
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
}
	
void ParticleBase::CreateWVP()
{
	particleResource_ = CreateBufferResource(dxCommon->GetDevice(), sizeof(ParticleForGPUData)*kNumMaxInstance_);
	particleResource_->Map(0, nullptr, reinterpret_cast<void**>(&particleData_));
	
	for(int i = 0; i < kNumMaxInstance_; i++){
		particleData_[i].WVP = MakeIdentityMatrix();
		particleData_[i].World = MakeIdentityMatrix();
		particleData_[i].color = {1,1,1,1};
	}

	//SRV作成
	D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc{};
	instancingSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	instancingSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancingSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	instancingSrvDesc.Buffer.FirstElement = 0;
	instancingSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	instancingSrvDesc.Buffer.NumElements = kNumMaxInstance_;
	instancingSrvDesc.Buffer.StructureByteStride = sizeof(ParticleForGPUData);
	instancingSrvHandleCPU_ = GetCPUDescriptorHandle(dxCommon->GetSRVDescriptorHeap(), dxCommon->GetDescriptorSizeSRV(), 1);
	instancingSrvHandleGPU_ = GetGPUDescriptorHandle(dxCommon->GetSRVDescriptorHeap(), dxCommon->GetDescriptorSizeSRV(), 1);
	dxCommon->GetDevice()->CreateShaderResourceView(particleResource_.Get(), &instancingSrvDesc, instancingSrvHandleCPU_);
}


void ParticleBase::ApplyGlobalVariablesInitialize()
{
#ifdef _DEBUG
	//GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	////グループを追加
	//GlobalVariables::GetInstance()->CreateGroup(name_);
	//globalVariables->AddItem(name_, "0.translate", baseParticle_.transform.translate);
	//globalVariables->AddItem(name_, "1.rotate", baseParticle_.transform.rotation);
	//globalVariables->AddItem(name_, "2.scale", baseParticle_.transform.scale);
#endif // _DEBUG
}

void ParticleBase::ApplyGlobalVariablesUpdate()
{
#ifdef _DEBUG
	/*GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	baseParticle_.transform.translate = globalVariables->GetVector3Value(name_, "0.translate");
	baseParticle_.transform.rotation = globalVariables->GetVector3Value(name_, "1.rotate");
	baseParticle_.transform.scale = globalVariables->GetVector3Value(name_, "2.scale");*/
#endif // _DEBUG
}
