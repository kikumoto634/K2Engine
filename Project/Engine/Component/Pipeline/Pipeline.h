#pragma once
#include <wrl.h>
#include <dxcapi.h>

#include "DirectXCommon.h"

#include "BlendSetting.h"

class Pipeline
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	template <class T> using vector = std::vector<T>;

public:
	~Pipeline(){
		delete blendSetting_;
	}

	void Create(
		std::wstring vsPath, std::wstring psPath,			//シェーダパス
		vector<D3D12_ROOT_PARAMETER> rootParameter,			//ルートパラメータ
		vector<D3D12_STATIC_SAMPLER_DESC> staticSampler,	//サンプラー
		vector<D3D12_INPUT_ELEMENT_DESC> inputLayoutDesc,	//インプットレイアウト
		D3D12_FILL_MODE fillMode = D3D12_FILL_MODE_SOLID,	//描画種類
		D3D12_PRIMITIVE_TOPOLOGY_TYPE pipelinePrimitiveTopology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
		BlendSetting::BlendMode blendMode = BlendSetting::kBlendModeNormal,
		D3D12_CULL_MODE cullingMode = D3D12_CULL_MODE_BACK
	);

	void DepthStencilSet(bool enable = true, D3D12_DEPTH_WRITE_MASK writeMask = D3D12_DEPTH_WRITE_MASK_ALL, D3D12_COMPARISON_FUNC func = D3D12_COMPARISON_FUNC_LESS_EQUAL);

	//Getter
	const IDxcUtils* GetDxcUtils() {return dxcUtils_.Get();};
	const IDxcCompiler3* GetDxcCompiler()	{return dxcCompiler_.Get();}
	const IDxcIncludeHandler* GetIncludeHandler()	{return includeHandler_.Get();}
	const D3D12_INPUT_LAYOUT_DESC  GetInputLayout()	{return inputLayoutDesc_;}


	inline ID3D12RootSignature* GetRootSignature()	const {return rootSignature_.Get();}
	inline ID3D12PipelineState* GetGraphicsPipelineState() const	{return graphicsPipelineState_.Get();}


private:
	//コンパイルシェーダー
	IDxcBlob* CompileShader(
		//ComplierするShaderファイルパス
		const std::wstring& filePath,
		//Compilerに使用するProfile
		const wchar_t* profile,
		//DXC
		IDxcUtils* dxcUtils,
		IDxcCompiler3* dxcCompiler,
		IDxcIncludeHandler* includeHandler
	);

#pragma region レンダリングパイプラインステート関連
	//DXCCompiler
	bool CreateDXCCompiler();
	//ルートシグネチャ
	bool CreateRootSignature();
	//インプットレイアウト
	bool CreateInputLayout();
	//ブレンドステート
	bool CreateBlendState();
	//ラスタライザステート
	bool CreateRasterizerState();
	//シェーダー読み込み
	bool LoadShader();
	//パイプラインステートオブジェクト
	bool CreatePipelineStateObject();
#pragma endregion


private:
	//Shaderファイル BasePath
	static const std::wstring basePath_;

private:
	HRESULT result_{};
	DirectXCommon* dxCommon_ = nullptr;
	BlendSetting* blendSetting_ = nullptr;

	D3D12_FILL_MODE fillMode_ = D3D12_FILL_MODE_SOLID;
	D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopology_ = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

#pragma region シェーダ関連
	std::wstring VSpath_;
	wchar_t* VSVersion_ = L"vs_6_0";

	std::wstring PSPath_;
	wchar_t* PSVersion_ = L"ps_6_0";
#pragma endregion

#pragma  region 生成物
	/// <summary>
	/// シェーダコンパイル用のDXC変数
	/// </summary>
	ComPtr<IDxcUtils> dxcUtils_;
	ComPtr<IDxcCompiler3> dxcCompiler_;
	ComPtr<IDxcIncludeHandler> includeHandler_;


	//ルートシグネチャ/パラメータ		: ShaderとResourceをどのように関連付けるかを示したオブジェクト
	vector<D3D12_ROOT_PARAMETER> rootParameters_;
	ComPtr<ID3D12RootSignature> rootSignature_;
	//サンプラー
	vector<D3D12_STATIC_SAMPLER_DESC> staticSamplers_;
	//インプットレイアウト	: VertexShaderへ渡す頂点データがどのようなものかを指定するオブジェクト
	vector<D3D12_INPUT_ELEMENT_DESC> inputElementDesc_;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_{};

	//各シェーダ情報
	ComPtr<IDxcBlob> vertexShaderBlob_;
	ComPtr<IDxcBlob> pixelShaderBlob_;
	//ブレンドディスク		: PixelShaderからの出力を画面にどのように書き込むか設定する項目
	BlendSetting::BlendMode blendMode_;
	D3D12_RENDER_TARGET_BLEND_DESC blendDesc_{};
	//ラスタライザ			: ラスタライザに対する処理
	D3D12_RASTERIZER_DESC rasterizerDesc_{};

	//DSVStencil 深度設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc_;

	//カリング
	D3D12_CULL_MODE cullingMode_ = D3D12_CULL_MODE_BACK;

	//パイプラインステート
	ComPtr<ID3D12PipelineState> graphicsPipelineState_;
#pragma endregion
};

