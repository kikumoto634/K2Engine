#pragma once
#include <memory>
#include <d3d12.h>
#include <vector>

#include "Pipeline.h"

#include "BlendSetting.h"

struct PipelineDatas{
	std::unique_ptr<Pipeline> pipeline_;

	std::vector<D3D12_ROOT_PARAMETER> rootParameters_;			//ルートパラメータ
	std::vector<D3D12_STATIC_SAMPLER_DESC> staticSamplers_;		//サンプラー
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDesc_;		//インプットレイアウト

	D3D12_PRIMITIVE_TOPOLOGY_TYPE pipelinePrimitiveTopology_ = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;	//パイプライン
	D3D_PRIMITIVE_TOPOLOGY commandPrimitiveTopology_ = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;				//コマンドリスト

	D3D12_CULL_MODE cullingMode = D3D12_CULL_MODE_BACK;	//カリング
	D3D12_FILL_MODE fillMode_ = D3D12_FILL_MODE_SOLID;	//塗りつぶし

	BlendSetting::BlendMode blendMode = BlendSetting::kBlendModeNormal;	//ブレンド

	std::string VSPath_;
	std::string PSPath_;
};