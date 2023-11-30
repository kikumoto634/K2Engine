#pragma once
#include <wrl.h>
#include <dxcapi.h>
#include <vector>

#include "DirectXCommon.h"
#include "Pipeline.h"

//通常Pipelineと同一
//DXC
//インプットレイアウト
//RootSignature
//PrimitiveTopology

//どうやってもらってくる?使用する?
//通常Pipelineと同じGeometryに持たせる
//呼び出しはGeometryの影描画関数を新しく作成
//影pipelineには、同様の物を受け取れるように、Geometry内でGetterでの受け取り

class ShadowPipeline
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	template <class T> using vector = std::vector<T>;

public:
	void Create(
		std::wstring vsPath,
		std::wstring psPath
	);

	//Getter
	ID3D12PipelineState* GetGraphicsPipelineState()	{return graphicsPipelineState_.Get();}

private:
	//コンパイルシェーダー
	static IDxcBlob* CompileShader(
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
	DirectXCommon* dxCommon = nullptr;

	std::wstring VSPath_;
	wchar_t* VSVersion_ = L"vs_6_0";

	std::wstring PSPath_;
	wchar_t* PSVersion_ = L"ps_6_0";

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
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_{};

	//各シェーダ情報
	ComPtr<IDxcBlob> vertexShaderBlob_;
	ComPtr<IDxcBlob> pixelShaderBlob_;
	//ブレンドディスク		: PixelShaderからの出力を画面にどのように書き込むか設定する項目
	D3D12_RENDER_TARGET_BLEND_DESC blendDesc_{};
	//ラスタライザ			: ラスタライザに対する処理
	D3D12_RASTERIZER_DESC rasterizerDesc_{};

	//パイプラインステート
	ComPtr<ID3D12PipelineState> graphicsPipelineState_;
};

