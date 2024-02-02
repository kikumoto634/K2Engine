#include "SpriteCommon.h"

using namespace std;

SpriteCommon* SpriteCommon::instance_ = nullptr;

SpriteCommon *SpriteCommon::GetInstance()
{
	if(!instance_){
		instance_ = new SpriteCommon;
		instance_->Initialize();
	}
	return instance_;
}

void SpriteCommon::Finalize()
{
	if(instance_){
		delete instance_;
	}
}



void SpriteCommon::Initialize()
{
	datas.pipeline_ = make_unique<Pipeline>();


	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;	//0から開始
	descriptorRange[0].NumDescriptors = 1;	//数は1
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;	//SRVを使用
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;		//Offsetを自動計算


	//ルートパラメータ設定
	std::vector<D3D12_ROOT_PARAMETER> rootParameters;
	rootParameters.resize(3);
	//PS
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		//CBV
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	//PixelShader使用
	rootParameters[0].Descriptor.ShaderRegister = 0;	//レジスタ番号 b0	
	//※RegisterとはShader上でのResource配置場所の情報　bというのは(ConstantBuffer)を意味
	//VS
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		//CBV
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;	//VertexShaderで使用
	rootParameters[1].Descriptor.ShaderRegister = 0;	//レジスタ番号 b0
	//SRV(テクスチャ		シェーダでは各ピクセルのことをいう)
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	//DescriptorTableに使用
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	//PixelShaderで使用
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;	//tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);	//Tableで利用する数


	//Sampler設定(シェーダーのPS SamplerState　シェーダでは画像のことをいう)
	std::vector<D3D12_STATIC_SAMPLER_DESC> staticSamplers;		//サンプラー
	staticSamplers.resize(1);
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;	//バイリニアフィルタ・
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	//0 ~ 1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;	
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;	//ありったけのMipMapを使用
	staticSamplers[0].ShaderRegister = 0;	//レジスタ番号0
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	//PixelShaderで使用

	
	//インプットレイアウト設定(頂点データでシェーダ内に送るデータたちのセマンティック名)
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDesc;		//インプットレイアウト
	inputElementDesc.resize(3);
	inputElementDesc[0].SemanticName = "POSITION";							//頂点シェーダーのセマンティック名
	inputElementDesc[0].SemanticIndex = 0;									//セマンティック番号
	inputElementDesc[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;			//float4 型
	inputElementDesc[0].InputSlot = 0;
	inputElementDesc[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDesc[0].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	inputElementDesc[0].InstanceDataStepRate = 0;

	inputElementDesc[1].SemanticName = "TEXCOORD";							//頂点シェーダーのセマンティック名
	inputElementDesc[1].SemanticIndex = 0;									//セマンティック番号
	inputElementDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;					//float4 型
	inputElementDesc[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDesc[2].SemanticName = "NORMAL";
	inputElementDesc[2].SemanticIndex = 0;
	inputElementDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDesc[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	//生成
	std::wstring vs = WindowsApp::ConvertString(datas.VSPath_);
	std::wstring ps = WindowsApp::ConvertString(datas.PSPath_);


	datas.rootParameters_ = rootParameters;
	datas.staticSamplers_ = staticSamplers;
	datas.inputElementDesc_ = inputElementDesc;


	datas.pipeline_->DepthStencilSet();
	datas.pipeline_->Create(
		vs,
		ps,
		rootParameters,
		staticSamplers,
		inputElementDesc,
		datas.fillMode_,
		datas.pipelinePrimitiveTopology_,
		datas.blendMode,
		datas.cullingMode
	);
}
