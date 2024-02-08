#include "GPUParticleCommon.h"

#include "DirectXCommon.h"

GPUParticleCommon* GPUParticleCommon::instance_ = nullptr;

GPUParticleCommon *GPUParticleCommon::GetInstance()
{
    if(!instance_){
        instance_ = new GPUParticleCommon();
        instance_->Initialize();
    }
    return instance_;
}

void GPUParticleCommon::Finalize()
{
    if(instance_){
        delete instance_;
    }
}

void GPUParticleCommon::Initialize()
{
    datas.pipeline_ = std::make_unique<Pipeline>();

	//SRV(InstancedID)
	D3D12_DESCRIPTOR_RANGE descriptorRangeForInstancing[1] = {};
	descriptorRangeForInstancing[SRV_WVP_RANGE].BaseShaderRegister = 0;
	descriptorRangeForInstancing[SRV_WVP_RANGE].NumDescriptors = 1;
	descriptorRangeForInstancing[SRV_WVP_RANGE].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRangeForInstancing[SRV_WVP_RANGE].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//ルートパラメータ設定
	std::vector<D3D12_ROOT_PARAMETER> rootParameters;
	rootParameters.resize(RootParamsNum);

	rootParameters[DESCRIPTOR_VERTEX_WVP].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;		//CBV
	rootParameters[DESCRIPTOR_VERTEX_WVP].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;	//VertexShaderで使用
	rootParameters[DESCRIPTOR_VERTEX_WVP].DescriptorTable.pDescriptorRanges = descriptorRangeForInstancing;	//レジスタ番号 b0
	rootParameters[DESCRIPTOR_VERTEX_WVP].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForInstancing);

	rootParameters[CBV_PIXEL_MATERIAL].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[CBV_PIXEL_MATERIAL].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[CBV_PIXEL_MATERIAL].Descriptor.ShaderRegister = 0;

	
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


	datas.rootParameters_ = rootParameters;
	datas.inputElementDesc_ = inputElementDesc;

	datas.VSPath_ = VSPath_;
	datas.PSPath_ = PSPath_;

	//生成
	std::wstring vs = WindowsApp::ConvertString(datas.VSPath_);
	std::wstring ps = WindowsApp::ConvertString(datas.PSPath_);

	datas.commandPrimitiveTopology_ = D3D10_PRIMITIVE_TOPOLOGY_POINTLIST;

	datas.fillMode_ = D3D12_FILL_MODE_WIREFRAME;
	datas.pipelinePrimitiveTopology_ = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	datas.blendMode = BlendSetting::kBlendModeNormal;
	datas.cullingMode = D3D12_CULL_MODE_NONE;

	datas.pipeline_->DepthStencilSet();
	datas.pipeline_->Create(
		vs,
		ps,
		rootParameters,
		{},
		inputElementDesc,
		datas.fillMode_,
		datas.pipelinePrimitiveTopology_,
		datas.blendMode,
		datas.cullingMode
	);
}

void GPUParticleCommon::Draw(DirectXCommon* dxCommon)
{
	dxCommon->GetCommandList()->SetGraphicsRootSignature(datas.pipeline_->GetRootSignature());
	dxCommon->GetCommandList()->SetPipelineState(datas.pipeline_->GetGraphicsPipelineState());
	dxCommon->GetCommandList()->IASetPrimitiveTopology(datas.commandPrimitiveTopology_);
}
