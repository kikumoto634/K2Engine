#include "GeometryBase.h"

#include "SpriteLoader.h"
#include "DescriptorHeap.h"

#include "BufferResource.h"
#include "BufferView.h"


void GeometryBase::Initialize(bool isIndexEnable)
{
	dxCommon = DirectXCommon::GetInstance();

	translate = {0,0,0};
	rotation = {0,0,0};
	scale = {1,1,1};

	isIndexDataEnable_ = isIndexEnable;

	//テクスチャSRV
	TextureSRVInitialize();

	//パイプライン
	pipeline_ = new Pipeline();
	PipelineStateInitialize();	//パイプライン初期化

	//リソース
	CreateVertex();
	CreateIndex();
	CreateMaterial();
	CreateWVP();
	CreateLight();
}

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
	dxCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//マテリアルのconstBufferの場所を設定
	dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, constResource_->GetGPUVirtualAddress());
	//行列のwvpBufferの場所を設定 ※RootParameter[1]に対してCBVの設定
	dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
	//SRV(テクスチャ)のDescriptorTableの先頭を設定 2はRootParamterのインデックスRootParamter[2]
	dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU_);
	//Light
	dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());

	//描画
	isIndexDataEnable_ ? 
		dxCommon->GetCommandList()->DrawIndexedInstanced(indexNum_,1,0,0,0) : 
		dxCommon->GetCommandList()->DrawInstanced(vertNum_,1,0,0);
}


void GeometryBase::TextureSRVInitialize()
{
	//画像読み込み
	DirectX::ScratchImage mipImages = SpriteLoader::LoadTexture(texturePath_);
	const DirectX::TexMetadata& metaData = mipImages.GetMetadata();
	ID3D12Resource* textureResource = SpriteLoader::CreateTextureResource(dxCommon->GetDevice(), metaData);
	SpriteLoader::UploadTextureData(textureResource, mipImages);

	//画像SRV生成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};	//MetaDataを基に設定
	srvDesc.Format = metaData.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(metaData.mipLevels);
	//画像SRVを使用するHeapの場所決め
	textureSrvHandleCPU_ = GetCPUDescriptorHandle(dxCommon->GetSRVDescriptorHeap(), dxCommon->GetDescriptorSIzeSRV(), 1);
	textureSrvHandleGPU_ = GetGPUDescriptorHandle(dxCommon->GetSRVDescriptorHeap(), dxCommon->GetDescriptorSIzeSRV(), 1);
	//画像SRV生成
	dxCommon->GetDevice()->CreateShaderResourceView(textureResource, &srvDesc, textureSrvHandleCPU_);
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
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;	//0から開始
	descriptorRange[0].NumDescriptors = 1;	//数は1
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;	//SRVを使用
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;		//Offsetを自動計算

	//ルートパラメータ設定
	rootParameters_.resize(4);
	//PS
	rootParameters_[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		//CBV
	rootParameters_[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	//PixelShader使用
	rootParameters_[0].Descriptor.ShaderRegister = 0;	//レジスタ番号 b0	
	//※RegisterとはShader上でのResource配置場所の情報　bというのは(ConstantBuffer)を意味
	//VS
	rootParameters_[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		//CBV
	rootParameters_[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;	//VertexShaderで使用
	rootParameters_[1].Descriptor.ShaderRegister = 0;	//レジスタ番号 b0
	//SRV(テクスチャ		シェーダでは各ピクセルのことをいう)
	rootParameters_[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	//DescriptorTableに使用
	rootParameters_[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	//PixelShaderで使用
	rootParameters_[2].DescriptorTable.pDescriptorRanges = descriptorRange;	//tableの中身の配列を指定
	rootParameters_[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);	//Tableで利用する数
	//Light(PS)
	rootParameters_[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		//CBV
	rootParameters_[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	//PixelShader
	rootParameters_[3].Descriptor.ShaderRegister = 1;	//レジスタ番号 b1


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
	pipeline_->Create(
		L"Object3D/Object3D.VS.hlsl",
		L"Object3D/Object3D.PS.hlsl",
		rootParameters_,
		staticSamplers_,
		inputElementDesc_,
		D3D12_FILL_MODE_SOLID
	);
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
	constResource_ = CreateBufferResource(dxCommon->GetDevice(), sizeof(Material));

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

void GeometryBase::CreateLight()
{
	directionalLightResource_= CreateBufferResource(dxCommon->GetDevice(), sizeof(DirectionalLight));

	directionalLightResource_->Map(0,nullptr,reinterpret_cast<void**>(&directionalLightData_));

	directionalLightData_->color = lightColor_;
	directionalLightData_->direction = lightDirection_;
	directionalLightData_->intensity = lightIntensity;
}
