#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <vector>

struct DescriptorSRVData{
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> Heap;
	std::vector<bool> CPUFlags;
	std::vector<bool> GPUFlags;
};

struct DescriptorRTVData{
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> Heap;
	std::vector<bool> CPUFlags;
	std::vector<bool> GPUFlags;
};

struct DescriptorDSVData{
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> Heap;
	std::vector<bool> CPUFlags;
	std::vector<bool> GPUFlags;
};