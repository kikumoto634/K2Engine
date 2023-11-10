#include "Object3D.h"
#include "SpriteLoader.h"

Object3D* Object3D::instance_ = nullptr;

Object3D *Object3D::Create()
{
	if(instance_==nullptr){
		instance_ = new Object3D();
	}
	instance_->Initialize();
	return instance_;
}

void Object3D::Initialize()
{
	dxCommon_ = DirectXCommon::GetInstance();

	//画像読み込み
	DirectX::ScratchImage mipImages = SpriteLoader::LoadTexture("uvChecker.png");
	const DirectX::TexMetadata& metaData = mipImages.GetMetadata();
	ID3D12Resource*textureResource = SpriteLoader::CreatetextureResource(dxCommon_->GetDevice(), metaData);
	SpriteLoader::UploadTextureData(textureResource, mipImages);

	{
		//MetaDataを基にSRV設定
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = metaData.format;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = UINT(metaData.mipLevels);

		//SRVを使用するHeapの場所決め
		D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = dxCommon_->GetSRVDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
		textureSrvHandleGPU = dxCommon_->GetSRVDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
		//先頭はImGuiなのでずらす
		textureSrvHandleCPU.ptr += dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		textureSrvHandleGPU.ptr += dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		//SRVの生成
		dxCommon_->GetDevice()->CreateShaderResourceView(textureResource, &srvDesc, textureSrvHandleCPU);
	}
		

	pipeline_ = new Pipeline();
	PipelineInitialize();

	//頂点バッファ作成
	assert(SUCCEEDED(CreateVertex()));
	//定数バッファ作成
	assert(SUCCEEDED(CreateConstant()));
	assert(SUCCEEDED(CreateWVP()));
}

void Object3D::PipelineInitialize()
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
	rootParameters_.resize(3);
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


	//Sampler設定(シェーダーのPS SamplerState　シェーダでは画像のことをいう)
	staticSamplers.resize(1);
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;	//バイリニアフィルタ・
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	//0 ~ 1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;	//ありったけのMipMapを使用
	staticSamplers[0].ShaderRegister = 0;	//レジスタ番号0
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	//PixelShaderで使用

	
	//インプットレイアウト設
	inputElementDescs_.resize(2);
	inputElementDescs_[0].SemanticName = "POSITION";							//頂点シェーダーのセマンティック名
	inputElementDescs_[0].SemanticIndex = 0;									//セマンティック番号
	inputElementDescs_[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;				//float4 型
	inputElementDescs_[0].InputSlot = 0;
	inputElementDescs_[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs_[0].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	inputElementDescs_[0].InstanceDataStepRate = 0;

	inputElementDescs_[1].SemanticName = "TEXCOORD";							//頂点シェーダーのセマンティック名
	inputElementDescs_[1].SemanticIndex = 0;									//セマンティック番号
	inputElementDescs_[1].Format = DXGI_FORMAT_R32G32_FLOAT;				//float4 型
	inputElementDescs_[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	//生成
	pipeline_->Create(
		L"Object3D/Object3D.VS.hlsl",
		L"Object3D/Object3D.PS.hlsl",
		rootParameters_,
		inputElementDescs_,
		staticSamplers,
		D3D12_FILL_MODE_SOLID
	);
}

void Object3D::Draw(Matrix4x4 viewProjectionMatrix)
{
	transform_.rotation.y += 0.03f;

	//更新情報
	Matrix4x4 worldViewProjectionMatrix = transform_.GetWorldMatrix() * viewProjectionMatrix;
	*wvpData = worldViewProjectionMatrix;

	//ルートシグネチャ設定 PSOに設定しいているが別途設定が必要
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(pipeline_->GetRootSignature());
	dxCommon_->GetCommandList()->SetPipelineState(pipeline_->GetGraphicsPipelineState());	//PSO設定
	dxCommon_->GetCommandList()->IASetVertexBuffers(0,1,&vertexBufferView_);		//VBV設定

	//形状設定、PSOに設定しているのとは別
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//マテリアルのconstBufferの場所を設定
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, constResource_->GetGPUVirtualAddress());
	//行列のwvpBufferの場所を設定 ※RootParameter[1]に対してCBVの設定
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
	//SRV(テクスチャ)のDescriptorTableの先頭を設定 2はRootParamterのインデックスRootParamter[2]
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);

	//描画
	dxCommon_->GetCommandList()->DrawInstanced(3,1,0,0);
}


//リソース作成
ID3D12Resource *Object3D::CreateBufferResource(size_t sizeInByte)
{
	ID3D12Resource* resource = nullptr;

	//リソース用のヒープ設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;	//UploadHeapを使用
	
	//リソース設定
	D3D12_RESOURCE_DESC ResourceDesc{};
	//バッファリソース。テクスチャの場合別の設定
	ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	ResourceDesc.Width = sizeInByte;		//リソースサイズ。今回はVector4を3頂点文
	//バッファの場合これらは1にする
	ResourceDesc.Height = 1;
	ResourceDesc.DepthOrArraySize = 1;
	ResourceDesc.MipLevels = 1;
	ResourceDesc.SampleDesc.Count = 1;
	//バッファの場合はこれにする決まり
	ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//リソースを作成
	result = dxCommon_->GetDevice()->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(result));

	return resource;
}

//ビュー作成
void Object3D::CreateBufferView(D3D12_VERTEX_BUFFER_VIEW& view, ID3D12Resource* resource, UINT sizeInByte, UINT strideInBytes)
{
	//リソースの先頭アドレスから使用
	view.BufferLocation = resource->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点3つ分のサイズ
	view.SizeInBytes = sizeInByte;
	//1頂点当たりのサイズ
	view.StrideInBytes = strideInBytes;
}

#pragma region 頂点リソース
bool Object3D::CreateVertex()
{
	//リソース
	vertexResource_ = CreateBufferResource(sizeof(VertexData)*3);
	//ビュー
	CreateBufferView(vertexBufferView_, vertexResource_.Get(), sizeof(VertexData)*3, sizeof(VertexData));

	//頂点リソースにデータを書き込む
	//書き込むためのアドレス取得
	vertexResource_->Map(0,nullptr,reinterpret_cast<void**>(&vertexData));
	
	//左上
	vertexData[0].position = {-0.5f, -0.5f, +0.0f, +1.0f};
	vertexData[0].texcoord = {+0.0f, +1.0f};
	//上
	vertexData[1].position = {+0.0f, +0.5f, +0.0f, +1.0f};
	vertexData[1].texcoord = {+0.5f, +0.0f};
	//右下
	vertexData[2].position = {+0.5f, -0.5f, +0.0f, +1.0f};
	vertexData[2].texcoord = {+1.0f, +1.0f};

	return true;
}
#pragma endregion

#pragma region 定数リソース
bool Object3D::CreateConstant()
{
	constResource_ = CreateBufferResource(sizeof(Vector4));

	constResource_->Map(0,nullptr,reinterpret_cast<void**>(&materialData));
	*materialData = color_;

	return true;
}
#pragma endregion

#pragma region 行列リソース
bool Object3D::CreateWVP()
{
	wvpResource_ = CreateBufferResource(sizeof(Matrix4x4));

	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	
	Matrix4x4 worldMatrix = worldMatrix.MakeIdentityMatrix();
	*wvpData = worldMatrix;

	return true;
}
#pragma endregion
