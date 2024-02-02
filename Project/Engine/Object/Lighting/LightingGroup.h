#pragma once
#include <wrl.h>
#include <d3d12.h>

#include "DirectXCommon.h"

#include "Transform.h"
#include "LightsData/DirectionalLightData.h"

class LightingGroup : Transform
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;


public:
	static LightingGroup* Create();
	static LightingGroup* GetInstance();

public:
    LightingGroup(const LightingGroup&) = delete;  // コピーコンストラクタをdelete
    LightingGroup& operator=(const LightingGroup&) = delete;  // コピー代入演算子をdelete

	void Initialize();
	void Update();


	//Getter
	ID3D12Resource* GetResource()	{return resource_.Get();}

private:
	LightingGroup() = default;  // デフォルトコンストラクタ

	void ApplyGlobalVariablesInitialize();
	void ApplyGlobalVariablesUpdate();

private:
	static LightingGroup* instance_;

private:
	DirectXCommon* dxCommon = nullptr;

	ComPtr<ID3D12Resource> resource_;	//リソース
	DirectionalLightData* data_ = nullptr;		//構造体

	Vector4 lightColor_ = {1,1,1,1};
	float lightIntensity_ = 1.0f;
};
