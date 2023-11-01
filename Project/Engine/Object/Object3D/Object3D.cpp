#include "Object3D.h"

#pragma comment(lib, "dxcompiler.lib")

Object3D* Object3D::instance_ = nullptr;

Object3D *Object3D::Create()
{
	if(instance_==nullptr){
		instance_ = new Object3D();
	}
	instance_->Initialize();
	return instance_;
}

void Object3D::Initialize()
{
	dxCommon_ = DirectXCommon::GetInstance();

	//DXC
	assert(SUCCEEDED(CreateDXCCompiler()));

	//パイプラインステート
	assert(SUCCEEDED(CreateRootSignature()));
	assert(SUCCEEDED(CreateInputLayout()));
	assert(SUCCEEDED(CreateBlendState()));
	assert(SUCCEEDED(CreateRasterizerState()));
	assert(SUCCEEDED(LoadShader()));
	assert(SUCCEEDED(CreatePipelineStateObject()));

	//頂点バッファ作成
	assert(SUCCEEDED(CreateVertex()));
	//定数バッファ作成
	assert(SUCCEEDED(CreateConstant()));
	assert(SUCCEEDED(CreateWVP()));
}

void Object3D::Draw(Matrix4x4 viewProjectionMatrix)
{

	transform_.rotation.y += 0.03f;

	//更新情報
	Matrix4x4 worldViewProjectionMatrix = transform_.GetWorldMatrix() * viewProjectionMatrix;
	*wvpData = worldViewProjectionMatrix;

	//ルートシグネチャ設定 PSOに設定しいているが別途設定が必要
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
	dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState_.Get());	//PSO設定
	dxCommon_->GetCommandList()->IASetVertexBuffers(0,1,&vertexBufferView_);		//VBV設定

	//形状設定、PSOに設定しているのとは別
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//マテリアルのconstBufferの場所を設定
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, constResource_->GetGPUVirtualAddress());
	//行列のwvpBufferの場所を設定 ※RootParameter[1]に対してCBVの設定
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());

	//描画
	dxCommon_->GetCommandList()->DrawInstanced(3,1,0,0);
}



IDxcBlob *Object3D::CompileShader(const std::wstring &filePath, const wchar_t *profile,IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler)
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


	//RootParameter作成 複数設定できるので配列
	// データそれぞれのBind情報、ConstantBufferのようにシェーダに情報を送る際に使用
	rootParameters_.resize(2);
	//PS
	rootParameters_[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		//CBV
	rootParameters_[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	//PixelShader使用
	rootParameters_[0].Descriptor.ShaderRegister = 0;	//レジスタ番号 b0	
	//※RegisterとはShader上でのResource配置場所の情報　bというのは(ConstantBuffer)を意味
	//VS
	rootParameters_[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		//CBV
	rootParameters_[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;	//VertexShaderで使用
	rootParameters_[1].Descriptor.ShaderRegister = 0;	//レジスタ番号 b0


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

bool Object3D::CreateInputLayout()
{
	//VertexShaderへ渡す頂点データがどのようなものかを指定するオブジェクト

	inputElementDescs_.resize(1);
	
	inputElementDescs_[0].SemanticName = "POSITION";							//頂点シェーダーのセマンティック名
	inputElementDescs_[0].SemanticIndex = 0;									//セマンティック番号
	inputElementDescs_[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;				//float4 型
	inputElementDescs_[0].InputSlot = 0;
	inputElementDescs_[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs_[0].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	inputElementDescs_[0].InstanceDataStepRate = 0;

	//Elementsのアドレスを取得しているため保存しておく必要あり
	inputLayoutDesc_.pInputElementDescs = inputElementDescs_.data();
	inputLayoutDesc_.NumElements = (UINT)inputElementDescs_.size();

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
	vertexShaderBlob_ = CompileShader(L"Resources/Shaders/Object3D/Object3D.VS.hlsl", L"vs_6_0", dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_.Get());
	assert(SUCCEEDED(result));
	pixelShaderBlob_ = CompileShader(L"Resources/Shaders/Object3D/Object3D.PS.hlsl", L"ps_6_0", dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_.Get());
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
#pragma endregion



//リソース作成
ID3D12Resource *Object3D::CreateBufferResource(size_t sizeInByte)
{
	ID3D12Resource* resource = nullptr;

	//リソース用のヒープ設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;	//UploadHeapを使用
	
	//リソース設定
	D3D12_RESOURCE_DESC ResourceDesc{};
	//バッファリソース。テクスチャの場合別の設定
	ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	ResourceDesc.Width = sizeInByte;		//リソースサイズ。今回はVector4を3頂点文
	//バッファの場合これらは1にする
	ResourceDesc.Height = 1;
	ResourceDesc.DepthOrArraySize = 1;
	ResourceDesc.MipLevels = 1;
	ResourceDesc.SampleDesc.Count = 1;
	//バッファの場合はこれにする決まり
	ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//リソースを作成
	result = dxCommon_->GetDevice()->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(result));

	return resource;
}

//ビュー作成
void Object3D::CreateBufferView(D3D12_VERTEX_BUFFER_VIEW& view, ID3D12Resource* resource, UINT sizeInByte, UINT strideInBytes)
{
	//リソースの先頭アドレスから使用
	view.BufferLocation = resource->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点3つ分のサイズ
	view.SizeInBytes = sizeInByte;
	//1頂点当たりのサイズ
	view.StrideInBytes = strideInBytes;
}

#pragma region 頂点リソース
bool Object3D::CreateVertex()
{
	//リソース
	vertexResource_ = CreateBufferResource(sizeof(Vector4)*3);
	//ビュー
	CreateBufferView(vertexBufferView_, vertexResource_.Get(), sizeof(Vector4)*3, sizeof(Vector4));

	//頂点リソースにデータを書き込む
	//書き込むためのアドレス取得
	vertexResource_->Map(0,nullptr,reinterpret_cast<void**>(&vertexData));
	
	//左上
	vertexData[0] = {-0.5f, -0.5f, +0.0f, +1.0f};
	//上
	vertexData[1] = {+0.0f, +0.5f, +0.0f, +1.0f};
	//右下
	vertexData[2] = {+0.5f, -0.5f, +0.0f, +1.0f};

	return true;
}
#pragma endregion

#pragma region 定数リソース
bool Object3D::CreateConstant()
{
	constResource_ = CreateBufferResource(sizeof(Vector4));

	constResource_->Map(0,nullptr,reinterpret_cast<void**>(&materialData));
	*materialData = color_;

	return true;
}
#pragma endregion

#pragma region 行列リソース
bool Object3D::CreateWVP()
{
	wvpResource_ = CreateBufferResource(sizeof(Matrix4x4));

	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	
	Matrix4x4 worldMatrix = worldMatrix.MakeIdentityMatrix();
	*wvpData = worldMatrix;

	return true;
}
#pragma endregion
