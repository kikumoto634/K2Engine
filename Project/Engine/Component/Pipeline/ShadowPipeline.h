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
		Pipeline* lPipeline,
		std::wstring vsPath
	);

	//Getter
	ID3D12PipelineState* GetGraphicsPipelineState()	{return graphicsPipelineState_.Get();}

private:
	DirectXCommon* dxCommon = nullptr;
	Pipeline* pipeline = nullptr;

	std::wstring VSPath_;
	wchar_t* VSVersion_ = L"vs_6_0";

	//各シェーダ情報
	ComPtr<IDxcBlob> vertexShaderBlob_;
	//ブレンドディスク		: PixelShaderからの出力を画面にどのように書き込むか設定する項目
	D3D12_RENDER_TARGET_BLEND_DESC blendDesc_{};
	//ラスタライザ			: ラスタライザに対する処理
	D3D12_RASTERIZER_DESC rasterizerDesc_{};

	//パイプラインステート
	ComPtr<ID3D12PipelineState> graphicsPipelineState_;
};

