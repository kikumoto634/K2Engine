#pragma once
#include <wrl.h>
#include <dxcapi.h>
#include <vector>

#include "DirectXCommon.h"

class Pipeline
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	template <class T> using vector = std::vector<T>;

public:
	void Create(
		std::wstring vsPath, std::wstring psPath,			//シェーダパス
		vector<D3D12_ROOT_PARAMETER> rootParameter,			//ルートパラメータ
		vector<D3D12_INPUT_ELEMENT_DESC> inputLayoutDesc,	//インプットレイアウト
		D3D12_FILL_MODE fillMode = D3D12_FILL_MODE_SOLID	//描画種類
	);

public:

	//Create
	//ルートシグネチャ

	//Getter
	ID3D12RootSignature* GetRootSignature()	{return rootSignature_.Get();}
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
	//Shaderファイル BasePath
	static const std::wstring basePath_;

private:
	HRESULT result{};
	DirectXCommon* dxCommon_ = nullptr;

	D3D12_FILL_MODE fillMode_ = D3D12_FILL_MODE_SOLID;

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
	//インプットレイアウト	: VertexShaderへ渡す頂点データがどのようなものかを指定するオブジェクト
	vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescs_;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_{};
	//各シェーダ情報
	ComPtr<IDxcBlob> vertexShaderBlob_;
	ComPtr<IDxcBlob> pixelShaderBlob_;
	//ブレンドディスク		: PixelShaderからの出力を画面にどのように書き込むか設定する項目
	D3D12_BLEND_DESC blendDesc_{};
	//ラスタライザ			: ラスタライザに対する処理
	D3D12_RASTERIZER_DESC rasterizerDesc_{};

	//パイプラインステート
	ComPtr<ID3D12PipelineState> graphicsPipelineState_;
#pragma endregion
};

