#include "GeometryCommon.h"

GeometryCommon* GeometryCommon::instance_ = nullptr;

GeometryCommon *GeometryCommon::GetInstance()
{
	if(!instance_){
		instance_ = new GeometryCommon();
		instance_->Initialize();
	}

	return instance_;
}

void GeometryCommon::Finalize()
{
	if(instance_){
		delete instance_;
	}
}

void GeometryCommon::Initialize()
{
	datas.pipeline_ = std::make_unique<Pipeline>();

	//SRV(Texture)
	D3D12_DESCRIPTOR_RANGE SRVDescriptorRange[1] = {};
	SRVDescriptorRange[SRV_TEXTURE_RANGE].BaseShaderRegister = 0;	//0から開始
	SRVDescriptorRange[SRV_TEXTURE_RANGE].NumDescriptors = 1;	//数は1
	SRVDescriptorRange[SRV_TEXTURE_RANGE].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	SRVDescriptorRange[SRV_TEXTURE_RANGE].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//ルートパラメータ設定
	std::vector<D3D12_ROOT_PARAMETER> rootParameters;
	rootParameters.resize(RootParamsNum);

	rootParameters[DESCRIPTOR_PIXEL_TEXTURE].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[DESCRIPTOR_PIXEL_TEXTURE].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[DESCRIPTOR_PIXEL_TEXTURE].DescriptorTable.pDescriptorRanges = SRVDescriptorRange;
	rootParameters[DESCRIPTOR_PIXEL_TEXTURE].DescriptorTable.NumDescriptorRanges = _countof(SRVDescriptorRange);

	rootParameters[CBV_ALL_LIGHT].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[CBV_ALL_LIGHT].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameters[CBV_ALL_LIGHT].Descriptor.ShaderRegister = 0;

	rootParameters[CBV_VERTEX_WVP].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[CBV_VERTEX_WVP].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameters[CBV_VERTEX_WVP].Descriptor.ShaderRegister = 1;

	rootParameters[CBV_PIXEL_MATERIAL].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[CBV_PIXEL_MATERIAL].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[CBV_PIXEL_MATERIAL].Descriptor.ShaderRegister = 2;

	rootParameters[CBV_PIXEL_CAMERA].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[CBV_PIXEL_CAMERA].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[CBV_PIXEL_CAMERA].Descriptor.ShaderRegister = 3;


	//Sampler設定(シェーダーのPS SamplerState　シェーダでは画像のことをいう)
	std::vector<D3D12_STATIC_SAMPLER_DESC> staticSamplers;
	staticSamplers.resize(StaticSamplesNum);
	staticSamplers[PIXEL_TEXTURE_SAMPLER].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	staticSamplers[PIXEL_TEXTURE_SAMPLER].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[PIXEL_TEXTURE_SAMPLER].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[PIXEL_TEXTURE_SAMPLER].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[PIXEL_TEXTURE_SAMPLER].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSamplers[PIXEL_TEXTURE_SAMPLER].MaxLOD = D3D12_FLOAT32_MAX;
	staticSamplers[PIXEL_TEXTURE_SAMPLER].ShaderRegister = 0;
	staticSamplers[PIXEL_TEXTURE_SAMPLER].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	
	//インプットレイアウト設定(頂点データでシェーダ内に送るデータたちのセマンティック名)
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDesc;
	inputElementDesc.resize(InputLayoutsNum);
	inputElementDesc[POSITION].SemanticName = "POSITION";
	inputElementDesc[POSITION].SemanticIndex = 0;
	inputElementDesc[POSITION].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDesc[POSITION].InputSlot = 0;
	inputElementDesc[POSITION].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDesc[POSITION].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	inputElementDesc[POSITION].InstanceDataStepRate = 0;

	inputElementDesc[UV].SemanticName = "TEXCOORD";	
	inputElementDesc[UV].SemanticIndex = 0;
	inputElementDesc[UV].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDesc[UV].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDesc[NORMAL].SemanticName = "NORMAL";
	inputElementDesc[NORMAL].SemanticIndex = 0;
	inputElementDesc[NORMAL].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDesc[NORMAL].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;


	datas.rootParameters_ = rootParameters;
	datas.staticSamplers_ = staticSamplers;
	datas.inputElementDesc_ = inputElementDesc;

	datas.VSPath_ = VSPath;
	datas.PSPath_ = PSPath;

	//生成
	std::wstring vs = WindowsApp::ConvertString(datas.VSPath_);
	std::wstring ps = WindowsApp::ConvertString(datas.PSPath_);

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
