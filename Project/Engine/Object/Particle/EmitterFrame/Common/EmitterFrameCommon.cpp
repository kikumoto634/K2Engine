#include "EmitterFrameCommon.h"

EmitterFrameCommon* EmitterFrameCommon::instance_ = nullptr;

EmitterFrameCommon *EmitterFrameCommon::GetInstance()
{
    if(!instance_){
        instance_ = new EmitterFrameCommon();
        instance_->Initialize();
    }
    return instance_;
}

void EmitterFrameCommon::Finalize()
{
    if(instance_){
        delete instance_;
    }
}

void EmitterFrameCommon::Initialize()
{
     datas.pipeline_ = std::make_unique<Pipeline>();

	//ルートパラメータ設定
	std::vector<D3D12_ROOT_PARAMETER> rootParameters;
	rootParameters.resize(RootParamsNum);

	rootParameters[CBV_VERTEX_WVP].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[CBV_VERTEX_WVP].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[CBV_VERTEX_WVP].Descriptor.ShaderRegister = 0;

	rootParameters[CBV_PIXEL_Material].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[CBV_PIXEL_Material].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[CBV_PIXEL_Material].Descriptor.ShaderRegister = 0;

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

	datas.commandPrimitiveTopology_ = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	datas.fillMode_ = D3D12_FILL_MODE_WIREFRAME;
	datas.pipelinePrimitiveTopology_ = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
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
