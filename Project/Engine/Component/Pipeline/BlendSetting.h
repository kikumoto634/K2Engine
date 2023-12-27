#pragma once
#include <d3d12.h>
class BlendSetting
{
public:
	enum BlendMode{
		//ブレンドなし
		kBlendModeNone,
		//αブレンド
		kBlendModeNormal,
		//加算
		kBlendModeAdd,
		//減算
		kBlendModeSubtract,
		//乗算
		kBlendModeMultiliy,
		//スクリーン
		kBlendModeScreen,
	};

public:
	static BlendSetting* GetInstance();

public:
	//設定
	void BlendSet(BlendMode mode);

	//ブレンド受け渡し
	D3D12_RENDER_TARGET_BLEND_DESC GetBlendDesc()	{return blendDesc_;}

private:
	//更新
	void BlendUpdate();

	void None();
	void Normal();
	void Add();
	void Sub();
	void Mul();
	void Screen();

private:
	static BlendSetting* instance;

private:
	//ブレンドディスク		: PixelShaderからの出力を画面にどのように書き込むか設定する項目
	D3D12_RENDER_TARGET_BLEND_DESC blendDesc_{};

	BlendMode blendMode = BlendMode::kBlendModeNormal;
};

