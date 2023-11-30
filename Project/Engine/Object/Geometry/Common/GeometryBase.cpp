#include "GeometryBase.h"

#include "SpriteLoader.h"
#include "DescriptorHeap.h"

#include "BufferResource.h"
#include "BufferView.h"

#include "LightingGroup.h"

void GeometryBase::Initialize(bool isIndexEnable)
{
	dxCommon = DirectXCommon::GetInstance();

	isIndexDataEnable_ = isIndexEnable;
	texture_ = SpriteLoader::SearchTexture(texturePath_);

	//描画方法
	if(primitiveType_ == PrimitiveType::LINE){
		pipelinePrimitiveTopology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		commandPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
	}

	//パイプライン
	pipeline_ = new Pipeline();
	//shadowPipeline_ = new ShadowPipeline();
	PipelineStateInitialize();	//パイプライン初期化

	//リソース
	CreateVertex();
	CreateIndex();
	CreateMaterial();
	CreateWVP();
}

/*void GeometryBase::ShadowDraw(Matrix4x4 viewProjectionMatrix)
{
	Matrix4x4 worldViewProjectionMatrix = GetWorldMatrix() * viewProjectionMatrix;
	wvpData_->WVP = worldViewProjectionMatrix;
	wvpData_->World = worldViewProjectionMatrix;


	//ルートシグネチャ設定 PSOに設定しいているが別途設定が必要
	dxCommon->GetCommandList()->SetGraphicsRootSignature(pipeline_->GetRootSignature());
	dxCommon->GetCommandList()->SetPipelineState(shadowPipeline_->GetGraphicsPipelineState());	//PSO設定
	dxCommon->GetCommandList()->IASetVertexBuffers(0,1,&vertexBufferView_);		//VBV設定
	if(isIndexDataEnable_)dxCommon->GetCommandList()->IASetIndexBuffer(&indexBufferView_);		//IBV設定

	//形状設定、PSOに設定しているのとは別
	dxCommon->GetCommandList()->IASetPrimitiveTopology(commandPrimitiveTopology);

	////マテリアルのconstBufferの場所を設定
	//dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, constResource_->GetGPUVirtualAddress());
	////行列のwvpBufferの場所を設定 ※RootParameter[1]に対してCBVの設定
	//dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
	////SRV(テクスチャ)のDescriptorTableの先頭を設定 2はRootParamterのインデックスRootParamter[2]
	//dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(2, texture_.srvHandleGPU_);
	////Light
	dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());

	//描画
	/*isIndexDataEnable_ ? 
		dxCommon->GetCommandList()->DrawIndexedInstanced(indexNum_,1,0,0,0) : 
		dxCommon->GetCommandList()->DrawInstanced(vertNum_,1,0,0);

}*/

void GeometryBase::Draw(Matrix4x4 viewProjectionMatrix)
{
	Matrix4x4 worldViewProjectionMatrix = GetWorldMatrix() * viewProjectionMatrix;
	wvpData_->WVP = worldViewProjectionMatrix;
	wvpData_->World = worldViewProjectionMatrix;


	//ルートシグネチャ設定 PSOに設定しいているが別途設定が必要
	dxCommon->GetCommandList()->SetGraphicsRootSignature(pipeline_->GetRootSignature());
	dxCommon->GetCommandList()->SetPipelineState(pipeline_->GetGraphicsPipelineState());	//PSO設定
	dxCommon->GetCommandList()->IASetVertexBuffers(0,1,&vertexBufferView_);		//VBV設定
	if(isIndexDataEnable_)dxCommon->GetCommandList()->IASetIndexBuffer(&indexBufferView_);		//IBV設定

	//形状設定、PSOに設定しているのとは別
	dxCommon->GetCommandList()->IASetPrimitiveTopology(commandPrimitiveTopology);


	//SRV(テクスチャ)のDescriptorTableの先頭を設定 2はRootParamterのインデックスRootParamter[2]
	dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(0, texture_.srvHandleGPU_);
	//Light
	dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, LightingGroup::GetInstance()->GetResource()->GetGPUVirtualAddress());
	//行列のwvpBufferの場所を設定 ※RootParameter[1]に対してCBVの設定
	dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(2, wvpResource_->GetGPUVirtualAddress());
	//マテリアルのconstBufferの場所を設定
	dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(3, constResource_->GetGPUVirtualAddress());

	//描画
	isIndexDataEnable_ ? 
		dxCommon->GetCommandList()->DrawIndexedInstanced(indexNum_,1,0,0,0) : 
		dxCommon->GetCommandList()->DrawInstanced(vertNum_,1,0,0);
}


void GeometryBase::PipelineStateInitialize()
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
	// 1~x / SRV(Texture)
	// 2+x / CSV(ALL)
	// 3+x / CSV(VERTEX)
	// 4+x / CSV(PIXEL)

	//SRV(Texture)
	D3D12_DESCRIPTOR_RANGE SRVDescriptorRange[1] = {};
	SRVDescriptorRange[0].BaseShaderRegister = 0;	//0から開始
	SRVDescriptorRange[0].NumDescriptors = 1;	//数は1
	SRVDescriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;	//SRVを使用
	SRVDescriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;		//Offsetを自動計算

	//ルートパラメータ設定
	rootParameters_.resize(4);
	//SRV(テクスチャ		シェーダでは各ピクセルのことをいう)
	rootParameters_[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	//DescriptorTableに使用
	rootParameters_[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	//PixelShaderで使用
	rootParameters_[0].DescriptorTable.pDescriptorRanges = SRVDescriptorRange;	//tableの中身の配列を指定
	rootParameters_[0].DescriptorTable.NumDescriptorRanges = _countof(SRVDescriptorRange);	//Tableで利用する数
	//ALL(ライト)
	rootParameters_[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		//CBV
	rootParameters_[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//PixelShader
	rootParameters_[1].Descriptor.ShaderRegister = 0;	//レジスタ番号 b0
	//※RegisterとはShader上でのResource配置場所の情報　bというのは(ConstantBuffer)を意味
	//VS(行列)
	rootParameters_[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		//CBV
	rootParameters_[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//VertexShaderで使用
	rootParameters_[2].Descriptor.ShaderRegister = 1;	//レジスタ番号 b1
	//PS(色)
	rootParameters_[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		//CBV
	rootParameters_[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	//PixelShader使用
	rootParameters_[3].Descriptor.ShaderRegister = 2;	//レジスタ番号 b1	


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
	inputElementDesc_.resize(3);
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

	inputElementDesc_[2].SemanticName = "NORMAL";
	inputElementDesc_[2].SemanticIndex = 0;
	inputElementDesc_[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDesc_[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	//生成
	std::wstring vs = WindowsApp::ConvertString(VSPath_);
	std::wstring ps = WindowsApp::ConvertString(PSPath_);
	pipeline_->Create(
		vs,
		ps,
		rootParameters_,
		staticSamplers_,
		inputElementDesc_,
		fillMode,
		pipelinePrimitiveTopology
	);


	////ルートパラメータ設定
	//shadowRootParameters_.resize(1);
	////行列
	//shadowRootParameters_[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		//CBV
	//shadowRootParameters_[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;	//VertexShaderで使用
	//shadowRootParameters_[0].Descriptor.ShaderRegister = 3;	//レジスタ番号 b1

	//shadowPipeline_->Create(
	//	pipeline_,
	//	L"Resources/Shaders/Object3D/Object3D.VS.Shadow.hlsl",
	//	shadowRootParameters_
	//);
}


void GeometryBase::CreateVertex()
{
	//リソース
	vertexResource_ = CreateBufferResource(dxCommon->GetDevice(), sizeof(VertexData)*vertNum_);
	//ビュー
	CreateBufferView(vertexBufferView_, vertexResource_.Get(), sizeof(VertexData)*vertNum_, sizeof(VertexData));

	//頂点リソースにデータを書き込む
	//書き込むためのアドレス取得
	vertexResource_->Map(0,nullptr,reinterpret_cast<void**>(&vertData_));
}

void GeometryBase::CreateIndex()
{
	if(!isIndexDataEnable_) return ;

	indexResource_ = CreateBufferResource(dxCommon->GetDevice(), sizeof(uint32_t)*indexNum_);

	CreateBufferView(indexBufferView_, indexResource_.Get(), sizeof(uint32_t)*indexNum_);

	//インデックスリソースにデータを書き込む
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
}

void GeometryBase::CreateMaterial()
{
	constResource_ = CreateBufferResource(dxCommon->GetDevice(), sizeof(GeometryMaterial));

	constResource_->Map(0,nullptr,reinterpret_cast<void**>(&materialData_));
	materialData_->color = color_;
	materialData_->enableLighting = isLightEnable;
	materialData_->uvTransform = materialData_->uvTransform.MakeIdentityMatrix();
}

void GeometryBase::CreateWVP()
{
	wvpResource_ = CreateBufferResource(dxCommon->GetDevice(), sizeof(TransformationMatrix));

	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
	
	Matrix4x4 worldMatrix = worldMatrix.MakeIdentityMatrix();
	wvpData_->WVP = worldMatrix;
	wvpData_->World = worldMatrix;
}