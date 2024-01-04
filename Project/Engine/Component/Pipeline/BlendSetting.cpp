#include "BlendSetting.h"

BlendSetting* BlendSetting::instance_ = nullptr;

BlendSetting *BlendSetting::GetInstance()
{
	if(!instance_)
	{
		instance_ = new BlendSetting();
		instance_->BlendUpdate();
	}
	return instance_;
}

void BlendSetting::BlendSet(BlendMode mode)
{
	blendMode_ = mode;
	BlendUpdate();
}

void BlendSetting::BlendUpdate()
{
	switch (blendMode_)
	{
	case BlendSetting::kBlendModeNone:
		None();
		break;
	case BlendSetting::kBlendModeNormal:
	default:
		Normal();
		break;
	case BlendSetting::kBlendModeAdd:
		Add();
		break;
	case BlendSetting::kBlendModeSubtract:
		Sub();
		break;
	case BlendSetting::kBlendModeMultiliy:
		Mul();
		break;
	case BlendSetting::kBlendModeScreen:
		Screen();
		break;
	}
}


void BlendSetting::None()
{
	//全ての色要素を書き込む
	blendDesc_.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	//レンダーターゲットのブレンド設定
	blendDesc_.BlendEnable = true;					//ブレンド有効

	blendDesc_.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc_.BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc_.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
}

void BlendSetting::Normal()
{
	//全ての色要素を書き込む
	blendDesc_.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	//レンダーターゲットのブレンド設定
	blendDesc_.BlendEnable = true;					//ブレンド有効

	blendDesc_.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc_.BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc_.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	blendDesc_.SrcBlendAlpha = D3D12_BLEND_ONE;		//ソースの値を100%使う
	blendDesc_.BlendOpAlpha = D3D12_BLEND_OP_ADD;	//加算
	blendDesc_.DestBlendAlpha = D3D12_BLEND_ZERO;	//デストの値を  0%使う
}

void BlendSetting::Add()
{
	//全ての色要素を書き込む
	blendDesc_.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	//レンダーターゲットのブレンド設定
	blendDesc_.BlendEnable = true;					//ブレンド有効

	blendDesc_.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc_.BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc_.DestBlend = D3D12_BLEND_ONE;

	blendDesc_.SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc_.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc_.DestBlendAlpha = D3D12_BLEND_ZERO;
}

void BlendSetting::Sub()
{
	//全ての色要素を書き込む
	blendDesc_.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	//レンダーターゲットのブレンド設定
	blendDesc_.BlendEnable = true;					//ブレンド有効

	blendDesc_.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc_.BlendOp = D3D12_BLEND_OP_SUBTRACT;
	blendDesc_.DestBlend = D3D12_BLEND_ONE;

	blendDesc_.SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc_.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc_.DestBlendAlpha = D3D12_BLEND_ZERO;
}

void BlendSetting::Mul()
{
	//全ての色要素を書き込む
	blendDesc_.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	//レンダーターゲットのブレンド設定
	blendDesc_.BlendEnable = true;					//ブレンド有効

	blendDesc_.SrcBlend = D3D12_BLEND_ZERO;
	blendDesc_.BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc_.DestBlend = D3D12_BLEND_SRC_COLOR;

	blendDesc_.SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc_.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc_.DestBlendAlpha = D3D12_BLEND_ZERO;
}

void BlendSetting::Screen()
{
	//全ての色要素を書き込む
	blendDesc_.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	//レンダーターゲットのブレンド設定
	blendDesc_.BlendEnable = true;					//ブレンド有効

	blendDesc_.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	blendDesc_.BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc_.DestBlend = D3D12_BLEND_ONE;

	blendDesc_.SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc_.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc_.DestBlendAlpha = D3D12_BLEND_ZERO;
}