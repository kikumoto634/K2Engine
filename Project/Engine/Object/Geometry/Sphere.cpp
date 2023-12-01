#include "Sphere.h"
#include <imgui.h>

Sphere *Sphere::Create(Transform transform)
{
	Sphere* instance = new Sphere(transform);
	instance->Initialize();
	instance->SphereVertexData();
	instance->SphereIndexData();
	instance->PipelineChange();
	return instance;
}

void Sphere::Update()
{
	ImGui::Text("Sphere");
	ImGui::DragFloat3("Pos", &this->transform.translate.x, 0.01f);
	ImGui::DragFloat3("Rot", &this->transform.rotation.x, 1.f);
	ImGui::DragFloat3("Scale", &this->transform.scale.x, 0.01f);
}

void Sphere::Draw(Matrix4x4 viewProjectionMatrix)
{
	//ルートシグネチャ設定 PSOに設定しいているが別途設定が必要
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootSignature(pipeline_->GetRootSignature());
	DirectXCommon::GetInstance()->GetCommandList()->SetPipelineState(pipeline_->GetGraphicsPipelineState());	//PSO設定

	//形状設定、PSOに設定しているのとは別
	DirectXCommon::GetInstance()->GetCommandList()->IASetPrimitiveTopology(commandPrimitiveTopology);

	GeometryBase::Draw(viewProjectionMatrix);
}

void Sphere::PipelineChange()
{
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

	pipelinePrimitiveTopology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	commandPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	fillMode = D3D12_FILL_MODE_SOLID;
	
	VSPath_ = "Object3D/Object3D.VS.hlsl";
	PSPath_ = "Object3D/Object3D.PS.OneColor.hlsl";

	//生成
	std::wstring vs = WindowsApp::ConvertString(VSPath_);
	std::wstring ps = WindowsApp::ConvertString(PSPath_);

	pipeline_ = new Pipeline();
	pipeline_->Create(
		vs,
		ps,
		rootParameters_,
		staticSamplers_,
		inputElementDesc_,
		fillMode,
		pipelinePrimitiveTopology
	);
}

void Sphere::SphereVertexData()
{
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
			vertData_[start].position.x = cos(lat) * cos(lon);
			vertData_[start].position.y = sin(lat);
			vertData_[start].position.z = cos(lat) * sin(lon);
			vertData_[start].position.w = 1.0f;
			vertData_[start].texcoord.x = u;
			vertData_[start].texcoord.y = v;
			vertData_[start].normal.x = vertData_[start].position.x;
			vertData_[start].normal.y = vertData_[start].position.y;
			vertData_[start].normal.z = vertData_[start].position.z;
			
			vertData_[start+1].position.x = cos(lat+kLatEvert) * cos(lon);
			vertData_[start+1].position.y = sin(lat+kLatEvert);
			vertData_[start+1].position.z = cos(lat+kLatEvert) * sin(lon);
			vertData_[start+1].position.w = 1.0f;
			vertData_[start+1].texcoord.x = u;
			vertData_[start+1].texcoord.y = v - uvLength;
			vertData_[start+1].normal.x = vertData_[start+1].position.x;
			vertData_[start+1].normal.y = vertData_[start+1].position.y;
			vertData_[start+1].normal.z = vertData_[start+1].position.z;

			vertData_[start+2].position.x = cos(lat) * cos(lon+kLonEvery);
			vertData_[start+2].position.y = sin(lat);
			vertData_[start+2].position.z = cos(lat) * sin(lon+kLonEvery);
			vertData_[start+2].position.w = 1.0f;
			vertData_[start+2].texcoord.x = u + uvLength;
			vertData_[start+2].texcoord.y = v;
			vertData_[start+2].normal.x = vertData_[start+2].position.x;
			vertData_[start+2].normal.y = vertData_[start+2].position.y;
			vertData_[start+2].normal.z = vertData_[start+2].position.z;

			
			vertData_[start+3].position.x = cos(lat+kLatEvert) * cos(lon+kLonEvery);
			vertData_[start+3].position.y = sin(lat+kLatEvert);
			vertData_[start+3].position.z = cos(lat+kLatEvert) * sin(lon+kLonEvery);
			vertData_[start+3].position.w = 1.0f;
			vertData_[start+3].texcoord.x = u + uvLength;
			vertData_[start+3].texcoord.y = v - uvLength;
			vertData_[start+3].normal.x = vertData_[start+3].position.x;
			vertData_[start+3].normal.y = vertData_[start+3].position.y;
			vertData_[start+3].normal.z = vertData_[start+3].position.z;

		}
	}
}

void Sphere::SphereIndexData()
{
	uint32_t lonIndex = 0;
	uint32_t latIndex = 0;
	for(latIndex = 0; latIndex < kSubdivision; ++latIndex){
		for(lonIndex = 0; lonIndex < kSubdivision; ++lonIndex){
			uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
			uint32_t vertStart = (latIndex * kSubdivision + lonIndex) * 4;

			//インデックス
			indexData_[start] = vertStart;		indexData_[start+1] = vertStart+1;	indexData_[start+2] = vertStart+2;
			indexData_[start+3] = vertStart+1;	indexData_[start+4] = vertStart+3;	indexData_[start+5] = vertStart+2;
		}
	}
}

