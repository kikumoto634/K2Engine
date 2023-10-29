#include "Object3D.h"

#include <d3d12.h>
#include <cassert>

#include "WindowsApp.h"

#pragma comment(lib, "dxcompiler.lib")

Object3D* Object3D::instance_ = nullptr;

Object3D *Object3D::Create()
{
	if(instance_==nullptr){
		instance_ = new Object3D();
		instance_->Initialize();
	}
	return instance_;
}

void Object3D::Initialize()
{
	dxCommon_ = DirectXCommon::GetInstance();

	assert(SUCCEEDED(CreateDXCCompiler()));

	assert(SUCCEEDED(CreateRootSignature()));
	assert(SUCCEEDED(CreateInputLayout()));
	assert(SUCCEEDED(CreateBlendState()));
	assert(SUCCEEDED(CreateRasterizerState()));
	assert(SUCCEEDED(LoadShader()));
	assert(SUCCEEDED(CreatePipelineStateObject()));

	assert(SUCCEEDED(CreateVertexResource()));
	assert(SUCCEEDED(CreateVertexBufferView()));
	assert(SUCCEEDED(VertexResourceUpload()));

	assert(SUCCEEDED(CreateViewport()));
	assert(SUCCEEDED(CreateScissor()));
}

void Object3D::Draw()
{
	dxCommon_->GetCommandList()->RSSetViewports(1,&viewport_);			//ビューポート
	dxCommon_->GetCommandList()->RSSetScissorRects(1,&scissorRect_);	//シザー矩形

	//ルートシグネチャ設定 PSOに設定しいているが別途設定が必要
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
	dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState_.Get());	//PSO設定
	dxCommon_->GetCommandList()->IASetVertexBuffers(0,1,&vertexBufferView_);		//VBV設定

	//形状設定、PSOに設定しているのとは別
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//描画
	dxCommon_->GetCommandList()->DrawInstanced(3,1,0,0);
}



IDxcBlob *Object3D::CompileShader(const std::wstring &filePath, const wchar_t *profile)
{
	//シェーダーコンパイルログ
	WindowsApp::Log(WindowsApp::ConvertString(std::format(L"Begin CompileShader, path:{}, profile:{}\n", filePath, profile)));
	//hlsl読み込み
	IDxcBlobEncoding* shaderSource = nullptr;
	result = dxcUtils_->LoadFile(filePath.c_str(), nullptr, &shaderSource);
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
	result = dxcCompiler_->Compile(
		&shaderSourceBuffer,			//読み込んだファイル
		arguments,						//コンパイルオプション
		_countof(arguments),			//コンパイルオプション数
		includeHandler_.Get(),			//include含まれた諸々
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



bool Object3D::CreateDXCCompiler()
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



bool Object3D::CreateRootSignature()
{
	//ShaderとResourceをどのように関連付けるかを示したオブジェクト

	//作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

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

bool Object3D::CreateInputLayout()
{
	//VertexShaderへ渡す頂点データがどのようなものかを指定するオブジェクト

	D3D12_INPUT_ELEMENT_DESC inputElementDescs[1] = {};
	inputElementDescs[0].SemanticName = "POSITION";							//頂点シェーダーのセマンティック名
	inputElementDescs[0].SemanticIndex = 0;									//セマンティック番号
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;			//float4 型
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	
	inputLayoutDesc_.pInputElementDescs = inputElementDescs;
	inputLayoutDesc_.NumElements = _countof(inputElementDescs);

	return true;
}

bool Object3D::CreateBlendState()
{
	//PixelShaderからの出力を画面にどのように書き込むか設定する項目

	//全ての色要素を書き込む
	blendDesc_.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return true;
}

bool Object3D::CreateRasterizerState()
{
	//ラスタライザに対する処理

	//裏面(時計回り)を表示しない
	rasterizerDesc_.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;

	return true;
}

bool Object3D::LoadShader()
{
	//シェーダの読み込み
	vertexShaderBlob_ = CompileShader(L"Resources/Shaders/Object3D/Object3D.VS.hlsl", L"vs_6_0");
	assert(SUCCEEDED(result));
	pixelShaderBlob_ = CompileShader(L"Resources/Shaders/Object3D/Object3D.PS.hlsl", L"ps_6_0");
	assert(SUCCEEDED(result));

	return true;
}

bool Object3D::CreatePipelineStateObject()
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



bool Object3D::CreateVertexResource()
{
	//頂点リソース用のヒープ設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;	//UploadHeapを使用
	
	//頂点リソース設定
	D3D12_RESOURCE_DESC vertexResourceDesc{};
	//バッファリソース。テクスチャの場合別の設定
	vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexResourceDesc.Width = sizeof(Vector4) * 3;		//リソースサイズ。今回はVector4を3頂点文
	//バッファの場合これらは1にする
	vertexResourceDesc.Height = 1;
	vertexResourceDesc.DepthOrArraySize = 1;
	vertexResourceDesc.MipLevels = 1;
	vertexResourceDesc.SampleDesc.Count = 1;
	//バッファの場合はこれにする決まり
	vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//頂点リソースを作成
	result = dxCommon_->GetDevice()->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertexResource_));
	assert(SUCCEEDED(result));

	return true;
}

bool Object3D::CreateVertexBufferView()
{
	//頂点バッファビュー作成
	//リソースの先頭アドレスから使用
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(Vector4)*3;
	//1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(Vector4);

	return true;
}

bool Object3D::VertexResourceUpload()
{
	//頂点リソースにデータを書き込む
	Vector4* vertexData = nullptr; 
	//書き込むためのアドレス取得
	vertexResource_->Map(0,nullptr,reinterpret_cast<void**>(&vertexData));
	
	//左上
	vertexData[0] = {-0.5f, -0.5f, +0.0f, +1.0f};
	//上
	vertexData[1] = {+0.0f, +0.5f, +0.0f, +1.0f};
	//右下
	vertexData[2] = {+0.5f, -0.5f, +0.0f, +0.0f};

	return true;
}



bool Object3D::CreateViewport()
{
	//クライアント領域のサイズと一緒に画面全体に表示
	viewport_.Width = (FLOAT)WindowsApp::kWindowWidth_;
	viewport_.Height = (FLOAT)WindowsApp::kWindowHeight_;
	viewport_.TopLeftX = 0;
	viewport_.TopLeftY = 0;
	viewport_.MinDepth = 0.0f;
	viewport_.MaxDepth = 1.0f;

	return true;
}

bool Object3D::CreateScissor()
{
	//基本的にビューポートと同じ矩形を構成する
	scissorRect_.left = 0;
	scissorRect_.right = WindowsApp::kWindowWidth_;
	scissorRect_.top = 0;
	scissorRect_.bottom = WindowsApp::kWindowHeight_;

	return true;
}

