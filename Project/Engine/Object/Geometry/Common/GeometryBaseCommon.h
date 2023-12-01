#pragma once
#include "DirectXCommon.h"
#include "Pipeline.h"
#include <vector>

#include <memory>

class GeometryBaseCommon{
public:
	void Initialize();
	void PipelineInitialize();

public:
	DirectXCommon* dxCommon = nullptr;
	
	std::unique_ptr<Pipeline> pipeline_;
	
	D3D_PRIMITIVE_TOPOLOGY commandPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST; //コマンドリスト

private:
	std::vector<D3D12_ROOT_PARAMETER> rootParameters_;			//ルートパラメータ
	std::vector<D3D12_STATIC_SAMPLER_DESC> staticSamplers_;		//サンプラー
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDesc_;		//インプットレイアウト
	D3D12_PRIMITIVE_TOPOLOGY_TYPE pipelinePrimitiveTopology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;;	//パイプライン
	D3D12_FILL_MODE fillMode = D3D12_FILL_MODE_SOLID;		//塗りつぶし
	std::string VSPath_ = "Object3D/Object3D.VS.hlsl";
	std::string PSPath_= "Object3D/Object3D.PS.Texture.hlsl";
};