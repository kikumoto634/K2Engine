#pragma once
#include <d3d12.h>
#include <cstdint>
#include <string>
#include "Vector2.h"

struct Texture{
	//テクスチャ名
	std::string name;

	//テクスチャパス
	std::string filePath;

	//テクスチャ番号
	uint32_t index;

	Vector2 size;

	//ハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU_;
	D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU_;
};