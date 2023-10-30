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


	//ResourceとHeap
	// Resourceはメモリ上で配置されるが、「どこ」のメモリに配置するのかは重要
	// ・GPUでよく使用するものはGPUに近いほど良い
	// ・CPUでよく使用するものはCPUに近いほど良い
	
	// これらを全体的に管理するのが、「Heap」 
	// Resourceはどの種類のHeap上に配置するのか決めてから作成する
	// ・Default	(デフォルト)
	// ・Upload		(アップロード)
	// ・Readback	(リードバック)

	/// <summary>
	/// リソース作成関数
	/// </summary>
	/// <param name="sizeInByte"> 使用するバイトサイズ </param>
	ID3D12Resource* CreateBufferResource(size_t sizeInByte);
	
	/// <summary>
	/// ビュー作成関数
	/// </summary>
	/// <param name="sizeInByte">使用するバイトサイズ</param>
	/// <param name="Num">使用する個数</param>
	void CreateBufferView(D3D12_VERTEX_BUFFER_VIEW& view, ID3D12Resource* resource, UINT sizeInByte, UINT strideInBytes);

#pragma region 頂点リソース/ビュー/更新
	// 頂点データ用のResource : VertexBuffer/VertexResource		※メモリ内に保存されているデータ
	// 頂点データ用のView     : VertexBufferView(VBV)			※Shaderへの入力頂点として処理する作業方法
	bool CreateVertex();
#pragma endregion

#pragma region 定数リソース/ビュー/更新
	// 定数データ用のResource : ConstBuffer/ConstResource	
	// 定数データ用のView	  : ConstBufferView(CBV)		
	bool CreateConstant();
#pragma endregion


#pragma region ViewportとScissor
	bool CreateViewport();
	bool CreateScissor();
#pragma endregion

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



	//頂点リソース
	ComPtr<ID3D12Resource> vertexResource_;
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	//定数リソース
	ComPtr<ID3D12Resource> constResource_;



	//ビューポート
	D3D12_VIEWPORT viewport_{};
	//シザー矩形
	D3D12_RECT scissorRect_{};
};

