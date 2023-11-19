#pragma once
#include <wrl.h>
#include <d3d12.h>


#include "DirectionalLight.h"

class LightingGroup
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	struct Light{
		DirectionalLight* direction;	//平行光源
	};

public:
	static LightingGroup* GetInstance();

public:
    LightingGroup(const LightingGroup&) = delete;  // コピーコンストラクタをdelete
    LightingGroup& operator=(const LightingGroup&) = delete;  // コピー代入演算子をdelete

private:
	LightingGroup() = default;  // デフォルトコンストラクタ


private:
	static LightingGroup* instance_;

private:
	ComPtr<ID3D12Resource> lightResource_;	//リソース
	Light* lightData_ = nullptr;		//構造体

	//平行光源
	DirectionalLight directional;
};

