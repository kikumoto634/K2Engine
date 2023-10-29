#pragma once
#include <dxcapi.h>
#include <wrl.h>

class Object3D
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	void Initialize();

private:
	HRESULT result{};

	ComPtr<IDxcUtils> dxcUtils_;
	ComPtr<IDxcCompiler3> dxcCompiler_;
	ComPtr<IDxcIncludeHandler> includeHandler_;
};

