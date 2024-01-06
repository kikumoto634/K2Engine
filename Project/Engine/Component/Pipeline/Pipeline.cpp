#include "Pipeline.h"
#include <cassert>

//ブレンドの処理
//マルチレンダリング対応も
//※コンピュートのシェーダなどが発生した際どうする


#pragma comment(lib, "dxcompiler.lib")

const std::wstring Pipeline::basePath_ = L"Resources/Shaders/";

void Pipeline::Create(
	std::wstring vsPath, std::wstring psPath,
	vector<D3D12_ROOT_PARAMETER> rootParameter,
	vector<D3D12_STATIC_SAMPLER_DESC> staticSampler,	
	vector<D3D12_INPUT_ELEMENT_DESC> inputLayoutDesc,
	D3D12_FILL_MODE fillMode,
	D3D12_PRIMITIVE_TOPOLOGY_TYPE pipelinePrimitiveTopology,
	BlendSetting::BlendMode blendMode,
	D3D12_CULL_MODE cullingMode
)
{
	dxCommon_ = DirectXCommon::GetInstance();

	//シェーダーパス
	VSpath_ = basePath_ + vsPath;
	PSPath_ = basePath_ + psPath;

	//RootParameter作成 複数設定できるので配列
	// データそれぞれのBind情報、ConstantBufferのようにシェーダに情報を送る際に使用
	rootParameters_ = rootParameter;

	//サンプラー
	staticSamplers_ = staticSampler;

	//インプットレイアウト設定
	inputElementDesc_ = inputLayoutDesc;

	//FillMode
	fillMode_ = fillMode;

	//Primitive Topology
	primitiveTopology_ = pipelinePrimitiveTopology;

	blendMode_ = blendMode;

	cullingMode_ = cullingMode;

	CreateDXCCompiler();

	CreateRootSignature();
	CreateInputLayout();
	CreateBlendState();
	CreateRasterizerState();
	LoadShader();
	CreatePipelineStateObject();
}

void Pipeline::DepthStencilSet(bool enable, D3D12_DEPTH_WRITE_MASK writeMask, D3D12_COMPARISON_FUNC func)
{
	depthStencilDesc_.DepthEnable = enable;
	depthStencilDesc_.DepthWriteMask = writeMask;
	depthStencilDesc_.DepthFunc = func;
}

IDxcBlob *Pipeline::CompileShader(const std::wstring &filePath, const wchar_t *profile, IDxcUtils *dxcUtils, IDxcCompiler3 *dxcCompiler, IDxcIncludeHandler *includeHandler)
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


#pragma region レンダリングパイプラインステート関連
bool Pipeline::CreateDXCCompiler()
{
	result_ = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	assert(SUCCEEDED(result_));
	result_ = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
	assert(SUCCEEDED(result_));

	//現時点でのincludeはしないが、includeに対応するための設定を行う
	result_ = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(result_));

	return true;
}

bool Pipeline::CreateRootSignature()
{
	//ShaderとResourceをどのように関連付けるかを示したオブジェクト

	//作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;


	//ルートパラメータ配列へのポインタ渡し
	descriptionRootSignature.pParameters = rootParameters_.data();		//ポインタ渡し
	descriptionRootSignature.NumParameters = (UINT)rootParameters_.size();	//長さ渡し
	//サンプラー配列へのポインタ渡し
	descriptionRootSignature.pStaticSamplers = staticSamplers_.data();
	descriptionRootSignature.NumStaticSamplers = (UINT)staticSamplers_.size();


	//シリアライズとしてバイナリにする
	ID3D10Blob* signatureBlob = nullptr;
	ID3D10Blob* errorBlob = nullptr;
	result_ = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if(FAILED(result_)){
		WindowsApp::Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	
	//バイナリをもとに作成
	result_ = dxCommon_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(result_));

	return true;
}

bool Pipeline::CreateInputLayout()
{
	//VertexShaderへ渡す頂点データがどのようなものかを指定するオブジェクト
	
	//Elementsのアドレスを取得しているため保存しておく必要あり
	inputLayoutDesc_.pInputElementDescs = inputElementDesc_.data();
	inputLayoutDesc_.NumElements = (UINT)inputElementDesc_.size();

	return true;
}

bool Pipeline::CreateBlendState()
{
	//PixelShaderからの出力を画面にどのように書き込むか設定する項目

	blendSetting_ = BlendSetting::GetInstance();
	blendSetting_->BlendSet(blendMode_);
	blendDesc_ = blendSetting_->GetBlendDesc();

	return true;
}

bool Pipeline::CreateRasterizerState()
{
	//ラスタライザに対する処理

	//裏面(時計回り)を表示しない
	rasterizerDesc_.CullMode = cullingMode_;
	//三角形の中を塗りつぶす
	rasterizerDesc_.FillMode = fillMode_;

	return true;
}

bool Pipeline::LoadShader()
{
	//シェーダの読み込み
	vertexShaderBlob_ = CompileShader(VSpath_, VSVersion_, dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_.Get());
	assert(SUCCEEDED(result_));
	pixelShaderBlob_ = CompileShader(PSPath_, PSVersion_, dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_.Get());
	assert(SUCCEEDED(result_));

	return true;
}

bool Pipeline::CreatePipelineStateObject()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	//RootSignature
	graphicsPipelineStateDesc.pRootSignature = rootSignature_.Get();
	
	//InputLayout
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc_;

	//DepthStencil
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc_;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	//VS, PS Blob
	graphicsPipelineStateDesc.VS = {vertexShaderBlob_.Get()->GetBufferPointer(), vertexShaderBlob_.Get()->GetBufferSize()};
	graphicsPipelineStateDesc.PS = {pixelShaderBlob_.Get()->GetBufferPointer(), pixelShaderBlob_.Get()->GetBufferSize()};

	//Blend
	graphicsPipelineStateDesc.BlendState.RenderTarget[0] = blendDesc_;

	//Rasterrize
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc_;

	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	//利用するトポロジ(形状)のタイプ
	graphicsPipelineStateDesc.PrimitiveTopologyType = primitiveTopology_;
;
	
	//どのように画面に色を打ち込むか設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//生成
	result_ = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState_));
	assert(SUCCEEDED(result_));

	return true;
}
#pragma endregion