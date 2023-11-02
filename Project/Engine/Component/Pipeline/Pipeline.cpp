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
	vector<D3D12_INPUT_ELEMENT_DESC> inputLayoutDesc,
	D3D12_FILL_MODE fillMode
)
{
	dxCommon_ = DirectXCommon::GetInstance();

	//シェーダーパス
	VSpath_ = basePath_ + vsPath;
	PSPath_ = basePath_ + psPath;

	//RootParameter作成 複数設定できるので配列
	// データそれぞれのBind情報、ConstantBufferのようにシェーダに情報を送る際に使用
	rootParameters_ = rootParameter;

	//インプットレイアウト設定
	inputElementDescs_ = inputLayoutDesc;

	//FillMode
	fillMode_ = fillMode;


	assert(SUCCEEDED(CreateDXCCompiler()));

	assert(SUCCEEDED(CreateRootSignature()));
	assert(SUCCEEDED(CreateInputLayout()));
	assert(SUCCEEDED(CreateBlendState()));
	assert(SUCCEEDED(CreateRasterizerState()));
	assert(SUCCEEDED(LoadShader()));
	assert(SUCCEEDED(CreatePipelineStateObject()));
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
	result = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	assert(SUCCEEDED(result));
	result = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
	assert(SUCCEEDED(result));

	//現時点でのincludeはしないが、includeに対応するための設定を行う
	result = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(result));

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


	//シリアライズとしてバイナリにする
	ID3D10Blob* signatureBlob = nullptr;
	ID3D10Blob* errorBlob = nullptr;
	result = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if(FAILED(result)){
		WindowsApp::Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	
	//バイナリをもとに作成
	result = dxCommon_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(result));

	return true;
}

bool Pipeline::CreateInputLayout()
{
	//VertexShaderへ渡す頂点データがどのようなものかを指定するオブジェクト
	
	//Elementsのアドレスを取得しているため保存しておく必要あり
	inputLayoutDesc_.pInputElementDescs = inputElementDescs_.data();
	inputLayoutDesc_.NumElements = (UINT)inputElementDescs_.size();

	return true;
}

bool Pipeline::CreateBlendState()
{
	//PixelShaderからの出力を画面にどのように書き込むか設定する項目

	//全ての色要素を書き込む
	blendDesc_.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return true;
}

bool Pipeline::CreateRasterizerState()
{
	//ラスタライザに対する処理

	//裏面(時計回り)を表示しない
	rasterizerDesc_.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc_.FillMode = fillMode_;

	return true;
}

bool Pipeline::LoadShader()
{
	//シェーダの読み込み
	vertexShaderBlob_ = CompileShader(VSpath_, VSVersion_, dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_.Get());
	assert(SUCCEEDED(result));
	pixelShaderBlob_ = CompileShader(PSPath_, PSVersion_, dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_.Get());
	assert(SUCCEEDED(result));

	return true;
}

bool Pipeline::CreatePipelineStateObject()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature_.Get();
	
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc_;

	graphicsPipelineStateDesc.VS = {vertexShaderBlob_.Get()->GetBufferPointer(), vertexShaderBlob_.Get()->GetBufferSize()};
	graphicsPipelineStateDesc.PS = {pixelShaderBlob_.Get()->GetBufferPointer(), pixelShaderBlob_.Get()->GetBufferSize()};

	graphicsPipelineStateDesc.BlendState = blendDesc_;

	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc_;

	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	//利用するトポロジ(形状)のタイプ
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//どのように画面に色を打ち込むか設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//生成
	result = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState_));
	assert(SUCCEEDED(result));

	return true;
}
#pragma endregion