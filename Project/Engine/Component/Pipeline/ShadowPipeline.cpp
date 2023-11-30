#include "ShadowPipeline.h"
#include <d3dx12.h>

void ShadowPipeline::Create(
	std::wstring vsPath,
	std::wstring psPath
)
{
	dxCommon = DirectXCommon::GetInstance();

	VSPath_ = vsPath;
	PSPath_ = psPath;

	CreateDXCCompiler();

	CreateRootSignature();
	CreateInputLayout();
	CreateBlendState();
	CreateRasterizerState();
	LoadShader();
	CreatePipelineStateObject();
}

IDxcBlob *ShadowPipeline::CompileShader(const std::wstring &filePath, const wchar_t *profile, IDxcUtils *dxcUtils, IDxcCompiler3 *dxcCompiler, IDxcIncludeHandler *includeHandler)
{
	HRESULT result{};

	//シェーダーコンパイルログ
	WindowsApp::Log(WindowsApp::ConvertString(std::format(L"Begin CompileShader, path:{}, profile:{}\n", filePath, profile)));
	//hlsl読み込み
	IDxcBlobEncoding* shaderSource = nullptr;
	result = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	assert(SUCCEEDED(result));

	//読み込んだファイルの内容を設定
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;	//utf-8文字コード通知


	LPCWSTR arguments[] = {
		filePath.c_str(),			//コンパイル対象hlsl
		L"-E", L"main",				//エントリーポイント指定
		L"-T", profile,				//shaderProfile設定
		L"-Zi", L"-Qembed_debug",	//デバック用の情報埋め込み
		L"-Od",						//最適化なし
		L"-Zpr"						//メモリレイアウトは行優先
	};

	//Shaderをコンパイル
	IDxcResult* shaderResult = nullptr;
	result = dxcCompiler->Compile(
		&shaderSourceBuffer,			//読み込んだファイル
		arguments,						//コンパイルオプション
		_countof(arguments),			//コンパイルオプション数
		includeHandler,			//include含まれた諸々
		IID_PPV_ARGS(&shaderResult)		//コンパイル結果
	);
	assert(SUCCEEDED(result));


	//警告・エラー発生時、ログ出力
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if(shaderError != nullptr && shaderError->GetStringLength() != 0){
		WindowsApp::Log(shaderError->GetStringPointer());
		assert(false);
	}


	//コンパイル結果から実行用のバイナリ部分を取得
	IDxcBlob* shaderBlob = nullptr;
	result = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(result));
	//成功時
	WindowsApp::Log(WindowsApp::ConvertString(std::format(L"Compile Succeeded, path:{}, profile:{}\n", filePath, profile)));
	//リソース破棄
	shaderSource->Release();
	shaderResult->Release();

	return shaderBlob;
}

bool ShadowPipeline::CreateDXCCompiler()
{
	HRESULT result;

	result = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	assert(SUCCEEDED(result));
	result = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
	assert(SUCCEEDED(result));

	//現時点でのincludeはしないが、includeに対応するための設定を行う
	result = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(result));

	return true;
}

bool ShadowPipeline::CreateRootSignature()
{
	//ShaderとResourceをどのように関連付けるかを示したオブジェクト

	HRESULT result;

	//デスクリプタレンジの設定
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV[1]{};
	descRangeSRV[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	//設定
	CD3DX12_ROOT_PARAMETER rootParam[1] = {};
	rootParam[0].InitAsDescriptorTable(1, &descRangeSRV[0], D3D12_SHADER_VISIBILITY_PIXEL);
	///スタティックサンプラー
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0,D3D12_FILTER_MIN_MAG_MIP_POINT);

	//ルートシグネチャ (テクスチャ、定数バッファなどシェーダーに渡すリソース情報をまとめたオブジェクト)
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Init_1_0(_countof(rootParam), rootParam,1, &samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	//シリアライズとしてバイナリにする
	ID3D10Blob* signatureBlob = nullptr;
	ID3D10Blob* errorBlob = nullptr;
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if(FAILED(result)){
		WindowsApp::Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	
	//バイナリをもとに作成
	result = dxCommon->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(result));

	return true;
}

bool ShadowPipeline::CreateInputLayout()
{
	D3D12_INPUT_ELEMENT_DESC inputElementDesc[2];
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


	inputLayoutDesc_.pInputElementDescs = inputElementDesc;
	inputLayoutDesc_.NumElements = sizeof(inputElementDesc);

	return true;
}

bool ShadowPipeline::CreateBlendState()
{	
	blendDesc_.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	blendDesc_.BlendEnable = false;
	blendDesc_.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc_.SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc_.DestBlendAlpha = D3D12_BLEND_ZERO;

	blendDesc_.SrcBlend = D3D12_BLEND_ONE;
	blendDesc_.DestBlend = D3D12_BLEND_ZERO;
	blendDesc_.BlendOp = D3D12_BLEND_OP_ADD;
	
	blendDesc_.LogicOpEnable = false;
	blendDesc_.LogicOp =D3D12_LOGIC_OP_CLEAR;

	return true;
}

bool ShadowPipeline::CreateRasterizerState()
{
	//ラスタライザに対する処理

	//裏面(時計回り)を表示しない
	rasterizerDesc_.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc_ = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

	return true;
}

bool ShadowPipeline::LoadShader()
{
	//シェーダの読み込み
	vertexShaderBlob_ = CompileShader(VSPath_, VSVersion_, dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_.Get());
	assert(SUCCEEDED(result));
	pixelShaderBlob_ = CompileShader(PSPath_, PSVersion_, dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_.Get());
	assert(SUCCEEDED(result));

	return true;
}

bool ShadowPipeline::CreatePipelineStateObject()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	//RootSignature
	graphicsPipelineStateDesc.pRootSignature = rootSignature_.Get();
	
	//InputLayout
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc_;

	//DepthStencil
	graphicsPipelineStateDesc.DepthStencilState.DepthEnable = true;
	graphicsPipelineStateDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	graphicsPipelineStateDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;

	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_R32_FLOAT;

	//VS, PS Blob
	graphicsPipelineStateDesc.VS = {vertexShaderBlob_.Get()->GetBufferPointer(), vertexShaderBlob_.Get()->GetBufferSize()};
	graphicsPipelineStateDesc.PS = {pixelShaderBlob_.Get()->GetBufferPointer(), pixelShaderBlob_.Get()->GetBufferSize()};

	//Blend
	graphicsPipelineStateDesc.BlendState.RenderTarget[0] = blendDesc_;

	//Rasterrize
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc_;

	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 0;

	//利用するトポロジ(形状)のタイプ
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
;
	
	//どのように画面に色を打ち込むか設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//生成
	HRESULT result = dxCommon->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState_));
	assert(SUCCEEDED(result));

	return true;
}
