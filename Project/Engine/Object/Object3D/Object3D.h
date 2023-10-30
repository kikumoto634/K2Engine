#pragma once
#include <wrl.h>
#include <dxcapi.h>
#include <vector>

#include "WindowsApp.h"
#include "DirectXCommon.h"

class Object3D
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	template <class T> using vector = std::vector<T>;

public:
	static Object3D* Create();

public:
	//描画
	void Draw();

private:
	void Initialize();

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



	//ResourceとHeap
	// Resourceはメモリ上で配置されるが、「どこ」のメモリに配置するのかは重要
	// ・GPUでよく使用するものはGPUに近いほど良い
	// ・CPUでよく使用するものはCPUに近いほど良い
	
	// これらを全体的に管理するのが、「Heap」 
	// Resourceはどの種類のHeap上に配置するのか決めてから作成する
	// ・Default	(デフォルト)
	// ・Upload		(アップロード)
	// ・Readback	(リードバック)

	//頂点リソース
	// 頂点データ用のResource : VertexBuffer/VertexResource		※メモリ内に保存されているデータ
	// 頂点データ用のView     : VertexBufferView(VBV)			※Shaderへの入力頂点として処理する作業方法
	bool CreateVertexResource();
	bool CreateVertexBufferView();
	bool VertexResourceUpload();



	//ViewportとScissor
	bool CreateViewport();
	bool CreateScissor();

private:
	static Object3D* instance_;

private:
	HRESULT result{};
	DirectXCommon* dxCommon_ = nullptr;

	/// <summary>
	/// シェーダコンパイル用のDXC変数
	/// </summary>
	ComPtr<IDxcUtils> dxcUtils_;
	ComPtr<IDxcCompiler3> dxcCompiler_;
	ComPtr<IDxcIncludeHandler> includeHandler_;


	//ルートシグネチャ		: ShaderとResourceをどのように関連付けるかを示したオブジェクト
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

	//頂点リソース
	ComPtr<ID3D12Resource> vertexResource_;
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	//ビューポート
	D3D12_VIEWPORT viewport_{};
	//シザー矩形
	D3D12_RECT scissorRect_{};
};

