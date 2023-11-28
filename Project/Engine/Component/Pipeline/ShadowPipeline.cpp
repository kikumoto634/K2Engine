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

	//作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;


	//ルートパラメータ配列へのポインタ渡し
	descriptionRootSignature.pParameters = rootParameters_.data();		//ポインタ渡し
	descriptionRootSignature.NumParameters = (UINT)rootParameters_.size();	//長さ渡し


	//シリアライズとしてバイナリにする
	ID3D10Blob* signatureBlob = nullptr;
	ID3D10Blob* errorBlob = nullptr;
	HRESULT result = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if(FAILED(result)){
		WindowsApp::Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	
	//バイナリをもとに作成
	result = dxCommon->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(result));


	//シェーダー
	vertexShaderBlob_ = pipeline->CompileShader(VSPath_, VSVersion_, pipeline->GetDxcUtils(), pipeline->GetDxcCompiler(), pipeline->GetIncludeHandler());


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
	pipelineStateDesc.pRootSignature = rootSignature_.Get();
	pipelineStateDesc.InputLayout = pipeline->GetInputLayout();
	
	pipelineStateDesc.SampleDesc.Count = 1;
	pipelineStateDesc.SampleDesc.Quality = 0;
	pipelineStateDesc.SampleMask = UINT_MAX;

	pipelineStateDesc.DepthStencilState.DepthEnable = true;
	pipelineStateDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	pipelineStateDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	pipelineStateDesc.DepthStencilState.StencilEnable = false;
	pipelineStateDesc.DepthStencilState.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
	pipelineStateDesc.DepthStencilState.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;

	pipelineStateDesc.DepthStencilState.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	pipelineStateDesc.DepthStencilState.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	pipelineStateDesc.DepthStencilState.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	pipelineStateDesc.DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

	pipelineStateDesc.DepthStencilState.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	pipelineStateDesc.DepthStencilState.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	pipelineStateDesc.DepthStencilState.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	pipelineStateDesc.DepthStencilState.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	pipelineStateDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	pipelineStateDesc.VS = {vertexShaderBlob_.Get()->GetBufferPointer(), vertexShaderBlob_.Get()->GetBufferSize()};
	pipelineStateDesc.BlendState.RenderTarget[0] = blendDesc_;
	pipelineStateDesc.BlendState.AlphaToCoverageEnable = false;
	pipelineStateDesc.BlendState.IndependentBlendEnable = false;
	pipelineStateDesc.RasterizerState = rasterizerDesc_;
	pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	result = dxCommon->GetDevice()->CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState_));
	assert(SUCCEEDED(result));
}
