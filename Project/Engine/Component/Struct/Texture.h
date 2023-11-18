#pragma once
#include <d3d12.h>
#include <cstdint>
#include <string>

struct Texture{
	//テクスチャ名
	std::string name;

	//テクスチャパス
	std::string filePath;

	//テクスチャ番号
	uint32_t index;

	//ハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU_;
	D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU_;
};