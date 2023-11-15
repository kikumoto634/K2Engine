#include "Object3D.h"
#include "SpriteLoader.h"
#include "DescriptorHeap.h"
#include <imgui.h>

Object3D *Object3D::Create()
{
	Object3D* instance_ = nullptr;
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
	DirectX::ScratchImage mipImages1 = SpriteLoader::LoadTexture("uvChecker.png");
	const DirectX::TexMetadata& metaData1 = mipImages1.GetMetadata();
	ID3D12Resource*textureResource1 = SpriteLoader::CreateTextureResource(dxCommon_->GetDevice(), metaData1);
	SpriteLoader::UploadTextureData(textureResource1, mipImages1);
	
	DirectX::ScratchImage mipImages2 = SpriteLoader::LoadTexture("monsterBall.png");
	const DirectX::TexMetadata& metaData2 = mipImages2.GetMetadata();
	ID3D12Resource*textureResource2 = SpriteLoader::CreateTextureResource(dxCommon_->GetDevice(), metaData2);
	SpriteLoader::UploadTextureData(textureResource2, mipImages2);

	//SRV作成
	{
		//MetaDataを基にSRV設定
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc1{};
		srvDesc1.Format = metaData1.format;
		srvDesc1.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc1.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc1.Texture2D.MipLevels = UINT(metaData1.mipLevels);

		//SRVを使用するHeapの場所決め
		//先頭はImGuiなのでずらす
		D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU1 = GetCPUDescriptorHandle(dxCommon_->GetSRVDescriptorHeap(), dxCommon_->GetDescriptorSIzeSRV(), 1);
		textureSrvHandleGPU1_ = GetGPUDescriptorHandle(dxCommon_->GetSRVDescriptorHeap(), dxCommon_->GetDescriptorSIzeSRV(), 1);
		//SRVの生成
		dxCommon_->GetDevice()->CreateShaderResourceView(textureResource1, &srvDesc1, textureSrvHandleCPU1);


		//MetaDataを基にSRV設定
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2{};
		srvDesc2.Format = metaData2.format;
		srvDesc2.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc2.Texture2D.MipLevels = UINT(metaData2.mipLevels);

		//SRVを使用するHeapの場所決め
		//先頭はImGuiなのでずらす
		D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU2 = GetCPUDescriptorHandle(dxCommon_->GetSRVDescriptorHeap(), dxCommon_->GetDescriptorSIzeSRV(), 2);
		textureSrvHandleGPU2_ = GetGPUDescriptorHandle(dxCommon_->GetSRVDescriptorHeap(), dxCommon_->GetDescriptorSIzeSRV(), 2);
		//SRVの生成
		dxCommon_->GetDevice()->CreateShaderResourceView(textureResource2, &srvDesc2, textureSrvHandleCPU2);
	}

	pipeline_ = new Pipeline();
	PipelineInitialize();

	//頂点バッファ作成
	assert(SUCCEEDED(CreateVertex()));
	//インデックスバッファ作成
	assert(SUCCEEDED(CreateIndex()));
	//定数バッファ作成
	assert(SUCCEEDED(CreateConstant()));
	//行列バッファ作成
	assert(SUCCEEDED(CreateWVP()));
	//Lightバッファ作成
	assert(SUCCEEDED(CreateDirectionalLight()));
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
	inputElementDescs_.resize(3);
	inputElementDescs_[0].SemanticName = "POSITION";							//頂点シェーダーのセマンティック名
	inputElementDescs_[0].SemanticIndex = 0;									//セマンティック番号
	inputElementDescs_[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;				//float4 型
	inputElementDescs_[0].InputSlot = 0;
	inputElementDescs_[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs_[0].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	inputElementDescs_[0].InstanceDataStepRate = 0;

	inputElementDescs_[1].SemanticName = "TEXCOORD";							//頂点シェーダーのセマンティック名
	inputElementDescs_[1].SemanticIndex = 0;									//セマンティック番号
	inputElementDescs_[1].Format = DXGI_FORMAT_R32G32_FLOAT;					//float4 型
	inputElementDescs_[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs_[2].SemanticName = "NORMAL";
	inputElementDescs_[2].SemanticIndex = 0;
	inputElementDescs_[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs_[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	//生成
	pipeline_->Create(
		L"Object3D/Object3D.VS.hlsl",
		L"Object3D/Object3D.PS.hlsl",
		rootParameters_,
		staticSamplers_,
		inputElementDescs_,
		D3D12_FILL_MODE_SOLID
	);
}

void Object3D::Update()
{
	{
		ImGui::Checkbox("useMonsterBall", &isUseMonsterBall);
	}

	//平行光源
	{
		float lightCol[4] = {directionalLightData->color.x,directionalLightData->color.y,directionalLightData->color.z,directionalLightData->color.w};
		float lightDir[3] = {directionalLightData->direction.x, directionalLightData->direction.y, directionalLightData->direction.z};
		float lightInt = directionalLightData->intensity;

		ImGui::DragFloat4("LightColor", lightCol, 0.1f, 0.0f, 1.0f);
		ImGui::DragFloat3("LightDirection", lightDir, 0.1f, -1.f, 1.f);
		ImGui::DragFloat("LightIntencity", &lightInt, 0.1f, 0.0f, 100.0f);

		directionalLightData->color = {lightCol[0],lightCol[1],lightCol[2], lightCol[3]};
		directionalLightData->direction = {lightDir[0],lightDir[1],lightDir[2]};
		directionalLightData->intensity = lightInt;
	}

	//UV
	{
		ImGui::DragFloat2("UVTranslate", &uvTransformSprite.translate.x, 0.01f, -10.f, 10.f);
		ImGui::DragFloat2("UVScale", &uvTransformSprite.scale.x, 0.01f,-10.f,10.f);
		ImGui::SliderAngle("UVRotate", &uvTransformSprite.rotation.z);
	}
}

void Object3D::Draw(Matrix4x4 viewProjectionMatrix)
{
	transform_.rotation.y += 0.01f;
	//更新情報
	Matrix4x4 worldViewProjectionMatrix = transform_.GetWorldMatrix() * viewProjectionMatrix;
	wvpData->WVP = worldViewProjectionMatrix;
	wvpData->World = worldViewProjectionMatrix;


	//Sprite用
	Matrix4x4 worldMatrixSprite = worldMatrixSprite.MakeAffineMatrix(transformSprite_.scale, transformSprite_.rotation, transformSprite_.translate);
	Matrix4x4 viewMatrixSprite = viewMatrixSprite.MakeIdentityMatrix();
	Matrix4x4 projectionMatrixSprite = projectionMatrixSprite.MakeOrthographicMatrix(0.0f,0.0f, (float)WindowsApp::kWindowWidth_,(float)WindowsApp::kWindowHeight_, 0.0f,100.0f);
	Matrix4x4 worldViewProjectionMatrixSprite = worldMatrixSprite * (viewMatrixSprite*projectionMatrixSprite);
	transformationMatrixDataSprite_->WVP = worldViewProjectionMatrixSprite;
	transformationMatrixDataSprite_->World = worldViewProjectionMatrixSprite;

	Matrix4x4 scaleSprite = scaleSprite.MakeScaleMatrix(uvTransformSprite.scale);
	Matrix4x4 rotZSprite = rotZSprite.MakeRotationZMatrix(uvTransformSprite.rotation.z);
	Matrix4x4 transSprite = transSprite.MakeTranslateMatrix(uvTransformSprite.translate);
	Matrix4x4 uvTransformMatrix = scaleSprite;
	uvTransformMatrix = uvTransformMatrix * rotZSprite;
	uvTransformMatrix = uvTransformMatrix * transSprite;
	materialDataSprite->uvTransform= uvTransformMatrix;


	//ルートシグネチャ設定 PSOに設定しいているが別途設定が必要
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(pipeline_->GetRootSignature());
	dxCommon_->GetCommandList()->SetPipelineState(pipeline_->GetGraphicsPipelineState());	//PSO設定
	dxCommon_->GetCommandList()->IASetVertexBuffers(0,1,&vertexBufferView_);		//VBV設定
	dxCommon_->GetCommandList()->IASetIndexBuffer(&indexBufferView);		//IBV設定

	//形状設定、PSOに設定しているのとは別
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//マテリアルのconstBufferの場所を設定
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, constResource_->GetGPUVirtualAddress());
	//行列のwvpBufferの場所を設定 ※RootParameter[1]に対してCBVの設定
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
	//SRV(テクスチャ)のDescriptorTableの先頭を設定 2はRootParamterのインデックスRootParamter[2]
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, isUseMonsterBall ? textureSrvHandleGPU1_ : textureSrvHandleGPU2_);
	//Light
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());

	//描画
	dxCommon_->GetCommandList()->DrawIndexedInstanced((kSubdivision*kSubdivision*6),1,0,0,0);


	//Sprite用
	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferViewSprite_);
	dxCommon_->GetCommandList()->IASetIndexBuffer(&indexBufferViewSprite);		//IBV設定
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, constResourceSprite_->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceSprite_->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU1_);
	dxCommon_->GetCommandList()->DrawIndexedInstanced(6,1,0,0,0);
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
	//使用するリソースのサイズは頂点引数分のサイズ
	view.SizeInBytes = sizeInByte;
	//1頂点当たりのサイズ
	view.StrideInBytes = strideInBytes;
}

void Object3D::CreateBufferView(D3D12_INDEX_BUFFER_VIEW &view, ID3D12Resource *resource, UINT sizeInByte)
{
	//リソースの先頭アドレスから使う
	view.BufferLocation = resource->GetGPUVirtualAddress();
	//使用するリソースのサイズはインデックスは引数分
	view.SizeInBytes = sizeInByte;
	//インデックスはuint32_tとする
	view.Format = DXGI_FORMAT_R32_UINT;
}

#pragma region 頂点リソース
bool Object3D::CreateVertex()
{
	//リソース
	vertexResource_ = CreateBufferResource(sizeof(VertexData)*(kSubdivision*kSubdivision*4));
	//ビュー
	CreateBufferView(vertexBufferView_, vertexResource_.Get(), sizeof(VertexData)*(kSubdivision*kSubdivision*4), sizeof(VertexData));

	//頂点リソースにデータを書き込む
	//書き込むためのアドレス取得
	vertexResource_->Map(0,nullptr,reinterpret_cast<void**>(&vertexData));
	
	//球体
	//PI円周率
	const float pi = 3.14159265f;
	//経度分割1つ分の角度 φ
	const float kLonEvery = pi * 2.0f / (float)kSubdivision;
	//緯度分割1つ分の角度 Θ
	const float kLatEvert = pi / (float)kSubdivision;

	//経度インデックス
	uint32_t lonIndex = 0;
	//緯度インデックス
	uint32_t latIndex = 0;

	//緯度の方向に分割
	for(latIndex = 0; latIndex < kSubdivision; ++latIndex){
		float lat = -pi / 2.0f + kLatEvert * latIndex;	//Θ
		//経度の方向に分割しながら絵を書く
		for(lonIndex = 0; lonIndex < kSubdivision; ++lonIndex){
			uint32_t start = (latIndex * kSubdivision + lonIndex) * 4;
			float lon = lonIndex * kLonEvery;	//φ

			float u = (float)lonIndex / (float)kSubdivision;
			float v = 1.0f - (float)latIndex / (float)kSubdivision;
			float uvLength = 1.0f / (float)kSubdivision;

			//頂点データを入力
			//一枚目
			vertexData[start].position.x = cos(lat) * cos(lon);
			vertexData[start].position.y = sin(lat);
			vertexData[start].position.z = cos(lat) * sin(lon);
			vertexData[start].position.w = 1.0f;
			vertexData[start].texcoord.x = u;
			vertexData[start].texcoord.y = v;
			vertexData[start].normal.x = vertexData[start].position.x;
			vertexData[start].normal.y = vertexData[start].position.y;
			vertexData[start].normal.z = vertexData[start].position.z;
			
			vertexData[start+1].position.x = cos(lat+kLatEvert) * cos(lon);
			vertexData[start+1].position.y = sin(lat+kLatEvert);
			vertexData[start+1].position.z = cos(lat+kLatEvert) * sin(lon);
			vertexData[start+1].position.w = 1.0f;
			vertexData[start+1].texcoord.x = u;
			vertexData[start+1].texcoord.y = v - uvLength;
			vertexData[start+1].normal.x = vertexData[start+1].position.x;
			vertexData[start+1].normal.y = vertexData[start+1].position.y;
			vertexData[start+1].normal.z = vertexData[start+1].position.z;

			vertexData[start+2].position.x = cos(lat) * cos(lon+kLonEvery);
			vertexData[start+2].position.y = sin(lat);
			vertexData[start+2].position.z = cos(lat) * sin(lon+kLonEvery);
			vertexData[start+2].position.w = 1.0f;
			vertexData[start+2].texcoord.x = u + uvLength;
			vertexData[start+2].texcoord.y = v;
			vertexData[start+2].normal.x = vertexData[start+2].position.x;
			vertexData[start+2].normal.y = vertexData[start+2].position.y;
			vertexData[start+2].normal.z = vertexData[start+2].position.z;

			
			vertexData[start+3].position.x = cos(lat+kLatEvert) * cos(lon+kLonEvery);
			vertexData[start+3].position.y = sin(lat+kLatEvert);
			vertexData[start+3].position.z = cos(lat+kLatEvert) * sin(lon+kLonEvery);
			vertexData[start+3].position.w = 1.0f;
			vertexData[start+3].texcoord.x = u + uvLength;
			vertexData[start+3].texcoord.y = v - uvLength;
			vertexData[start+3].normal.x = vertexData[start+3].position.x;
			vertexData[start+3].normal.y = vertexData[start+3].position.y;
			vertexData[start+3].normal.z = vertexData[start+3].position.z;
		}
	}



	//Sprite用
	vertexResourceSprite_ = CreateBufferResource(sizeof(VertexData)*4);
	CreateBufferView(vertexBufferViewSprite_, vertexResourceSprite_.Get(), sizeof(VertexData)*4, sizeof(VertexData));
	vertexResourceSprite_->Map(0,nullptr,reinterpret_cast<void**>(&vertexDataSprite_));
	//一枚目
	vertexDataSprite_[0].position = {0.0f, 360.0f, 0.0f, 1.0f};		//左下
	vertexDataSprite_[0].texcoord = {0.0f, 1.0f};
	vertexDataSprite_[0].normal = {0.0f, 0.0f, -1.0f};
	vertexDataSprite_[1].position = {0.0f, 0.0f, 0.0f, 1.0f};		//左上
	vertexDataSprite_[1].texcoord = {0.0f, 0.0f};
	vertexDataSprite_[1].normal = {0.0f, 0.0f, -1.0f};
	vertexDataSprite_[2].position = {640.0f, 360.0f, 0.0f, 1.0f};	//右下
	vertexDataSprite_[2].texcoord = {1.0f, 1.0f};
	vertexDataSprite_[2].normal = {0.0f, 0.0f, -1.0f};
	vertexDataSprite_[3].position = {640.0f, 0.0f, 0.0f, 1.0f};		//右上
	vertexDataSprite_[3].texcoord = {1.0f, 0.0f};
	vertexDataSprite_[3].normal = {0.0f, 0.0f, -1.0f};

	return true;
}
#pragma endregion

#pragma region インデックスリソース
bool Object3D::CreateIndex()
{
	indexResource_ = CreateBufferResource(sizeof(uint32_t)*(kSubdivision*kSubdivision*6));
	CreateBufferView(indexBufferView, indexResource_.Get(), sizeof(uint32_t)*(kSubdivision*kSubdivision*6));
	indexResource_->Map(0,nullptr, reinterpret_cast<void**>(&indexData_));


	//PI円周率
	const float pi = 3.14159265f;
	//経度分割1つ分の角度 φ
	const float kLonEvery = pi * 2.0f / (float)kSubdivision;
	//緯度分割1つ分の角度 Θ
	const float kLatEvert = pi / (float)kSubdivision;
	//経度インデックス
	uint32_t lonIndex = 0;
	//緯度インデックス
	uint32_t latIndex = 0;
	for(latIndex = 0; latIndex < kSubdivision; ++latIndex){
		float lat = -pi / 2.0f + kLatEvert * latIndex;	//Θ
		//経度の方向に分割しながら絵を書く
		for(lonIndex = 0; lonIndex < kSubdivision; ++lonIndex){
			uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;

			//インデックス
			indexData_[start] = start;		indexData_[start+1] = start+1;	indexData_[start+2] = start+2;
			indexData_[start+3] = start+1;	indexData_[start+4] = start+3;	indexData_[start+5] = start+2;
		}
	}


	//Sprite用
	indexResourceSprite_ = CreateBufferResource(sizeof(uint32_t)*6);

	CreateBufferView(indexBufferViewSprite, indexResourceSprite_.Get(), sizeof(uint32_t)*6);

	//インデックスリソースにデータを書き込む
	indexResourceSprite_->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite_));

	indexDataSprite_[0] = 0;	indexDataSprite_[1] = 1;	indexDataSprite_[2] = 2;
	indexDataSprite_[3] = 1;	indexDataSprite_[4] = 3;	indexDataSprite_[5] = 2;

	return true;
}
#pragma endregion

#pragma region 定数リソース
bool Object3D::CreateConstant()
{
	constResource_ = CreateBufferResource(sizeof(Material));

	constResource_->Map(0,nullptr,reinterpret_cast<void**>(&materialData));
	materialData->color = color_;
	materialData->enableLighting = true;
	materialData->uvTransform = materialData->uvTransform.MakeIdentityMatrix();


	//Sprite用
	constResourceSprite_ = CreateBufferResource(sizeof(Material));

	constResourceSprite_->Map(0,nullptr,reinterpret_cast<void**>(&materialDataSprite));
	materialDataSprite->color = color_;
	materialDataSprite->enableLighting = false;
	materialDataSprite->uvTransform =materialDataSprite->uvTransform.MakeIdentityMatrix();

	return true;
}
#pragma endregion

#pragma region 行列リソース
bool Object3D::CreateWVP()
{
	wvpResource_ = CreateBufferResource(sizeof(TransformationMatrix));

	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	
	Matrix4x4 worldMatrix = worldMatrix.MakeIdentityMatrix();
	wvpData->WVP = worldMatrix;
	wvpData->World = worldMatrix;


	//Sprite用
	transformationMatrixResourceSprite_ = CreateBufferResource(sizeof(TransformationMatrix));
	transformationMatrixResourceSprite_->Map(0,nullptr,reinterpret_cast<void**>(&transformationMatrixDataSprite_));
	Matrix4x4 worldMatrixSprite = worldMatrixSprite.MakeIdentityMatrix();
	transformationMatrixDataSprite_->WVP = worldMatrixSprite;
	transformationMatrixDataSprite_->World = worldMatrixSprite;

	return true;
}
#pragma endregion

#pragma region Lightリソース
bool Object3D::CreateDirectionalLight()
{
	directionalLightResource_= CreateBufferResource(sizeof(DirectionalLight));

	directionalLightResource_->Map(0,nullptr,reinterpret_cast<void**>(&directionalLightData));

	directionalLightData->color = {1.0f, 1.0f, 1.0f, 1.0f};
	directionalLightData->direction = {0.0f, -1.0f, 0.0f};
	directionalLightData->intensity = 1.0f;

	return true;
}
#pragma endregion
