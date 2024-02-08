#include "ParticleCommon.h"

ParticleCommon* ParticleCommon::instance_ = nullptr;

ParticleCommon *ParticleCommon::GetInstance()
{
    if(!instance_){
        instance_ = new ParticleCommon();
        instance_->Initialize();
    }
    return instance_;
}

void ParticleCommon::Finalize()
{
    if(instance_){
        delete instance_;
    }
}

void ParticleCommon::Initialize()
{
     datas.pipeline_ = std::make_unique<Pipeline>();

	//SRV(Texture)
	D3D12_DESCRIPTOR_RANGE SRVDescriptorRange[1] = {};
	SRVDescriptorRange[SRV_TEXTURE_RANGE].BaseShaderRegister = 0;	//0から開始
	SRVDescriptorRange[SRV_TEXTURE_RANGE].NumDescriptors = 1;	//数は1
	SRVDescriptorRange[SRV_TEXTURE_RANGE].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	SRVDescriptorRange[SRV_TEXTURE_RANGE].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//SRV(WVP)
	D3D12_DESCRIPTOR_RANGE descriptorRangeForInstancing[1] = {};
	descriptorRangeForInstancing[SRV_WVP_RANGE].BaseShaderRegister = 0;
	descriptorRangeForInstancing[SRV_WVP_RANGE].NumDescriptors = 1;
	descriptorRangeForInstancing[SRV_WVP_RANGE].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRangeForInstancing[SRV_WVP_RANGE].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//ルートパラメータ設定
	std::vector<D3D12_ROOT_PARAMETER> rootParameters;
	rootParameters.resize(RootParamsNum);

	rootParameters[DESCRIPTOR_PIXEL_TEXTURE].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[DESCRIPTOR_PIXEL_TEXTURE].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[DESCRIPTOR_PIXEL_TEXTURE].DescriptorTable.pDescriptorRanges = SRVDescriptorRange;
	rootParameters[DESCRIPTOR_PIXEL_TEXTURE].DescriptorTable.NumDescriptorRanges = _countof(SRVDescriptorRange);

	rootParameters[DESCRIPTOR_VERTEX_WVP].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	
	rootParameters[DESCRIPTOR_VERTEX_WVP].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[DESCRIPTOR_VERTEX_WVP].DescriptorTable.pDescriptorRanges = descriptorRangeForInstancing;
	rootParameters[DESCRIPTOR_VERTEX_WVP].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForInstancing);

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

	inputElementDesc[TEXCOORD].SemanticName = "TEXCOORD";	
	inputElementDesc[TEXCOORD].SemanticIndex = 0;
	inputElementDesc[TEXCOORD].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDesc[TEXCOORD].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;


	datas.rootParameters_ = rootParameters;
	datas.staticSamplers_ = staticSamplers;
	datas.inputElementDesc_ = inputElementDesc;

	datas.VSPath_ = VSPath_;
	datas.PSPath_ = PSPath_;

	//生成
	std::wstring vs = WindowsApp::ConvertString(datas.VSPath_);
	std::wstring ps = WindowsApp::ConvertString(datas.PSPath_);

	datas.commandPrimitiveTopology_ = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	datas.fillMode_ = D3D12_FILL_MODE_SOLID;
	datas.pipelinePrimitiveTopology_ = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	datas.blendMode = BlendSetting::kBlendModeAdd;
	datas.cullingMode = D3D12_CULL_MODE_BACK;

	datas.pipeline_->DepthStencilSet(true, D3D12_DEPTH_WRITE_MASK_ZERO);
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
