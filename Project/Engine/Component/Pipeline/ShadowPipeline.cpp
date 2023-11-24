#include "ShadowPipeline.h"

void ShadowPipeline::Create(
	Pipeline *lPipeline, 
	std::wstring vsPath, 
	vector<D3D12_ROOT_PARAMETER> rootParameter
)
{
	dxCommon = DirectXCommon::GetInstance();
	this->pipeline = lPipeline;

	VSPath_ = vsPath;

	//シェーダー
	vertexShaderBlob_ = pipeline->CompileShader(VSPath_, VSVersion_, pipeline->GetDxcUtils(), pipeline->GetDxcCompiler(), pipeline->GetIncludeHandler());

	//インプット

	//ラスタライザ
	rasterizerDesc_.CullMode = D3D12_CULL_MODE_NONE;
	rasterizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;
	rasterizerDesc_.FrontCounterClockwise = false;
	rasterizerDesc_.DepthBias = 0;
	rasterizerDesc_.DepthBiasClamp = 0;
	rasterizerDesc_.SlopeScaledDepthBias = 0;
	rasterizerDesc_.DepthClipEnable = true;
	rasterizerDesc_.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	rasterizerDesc_.AntialiasedLineEnable = false;
	rasterizerDesc_.MultisampleEnable = false;

	//ブレンドステート
	blendDesc_.BlendEnable = false;
	blendDesc_.SrcBlend = D3D12_BLEND_ONE;
	blendDesc_.DestBlend = D3D12_BLEND_ZERO;
	blendDesc_.BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc_.SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc_.DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc_.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc_.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc_.LogicOpEnable = false;
	blendDesc_.LogicOp =D3D12_LOGIC_OP_CLEAR;


	//生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc{};
	pipelineStateDesc.pRootSignature = pipeline->GetRootSignature();
	pipelineStateDesc.InputLayout = pipeline->GetInputLayout();
	//DepthStencil未定

	pipelineStateDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	pipelineStateDesc.VS = {vertexShaderBlob_.Get()->GetBufferPointer(), vertexShaderBlob_.Get()->GetBufferSize()};
	pipelineStateDesc.BlendState.RenderTarget[0] = blendDesc_;
	pipelineStateDesc.BlendState.AlphaToCoverageEnable = false;
	pipelineStateDesc.BlendState.IndependentBlendEnable = false;
	pipelineStateDesc.RasterizerState = rasterizerDesc_;
	pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	HRESULT result = dxCommon->GetDevice()->CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState_));
	assert(SUCCEEDED(result));
}
