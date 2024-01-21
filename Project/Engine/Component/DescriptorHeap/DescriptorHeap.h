#pragma once
#include <d3d12.h>
#include <cstdint>

#include "DescriptorData.h"

//ディスクリプタヒープ作成
ID3D12DescriptorHeap* CreateDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heaptype, UINT numDescriptors, bool shaderVisible);

//DescripotDataにHeapと各格納箇所の使用有無情報アリ
//自動で格納されていない箇所のハンドルを取得する

//ディスクリプタヒープハンドル取得
D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(DescriptorSRVData& descriptorHeap, uint32_t descriptorSize);
D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(DescriptorRTVData& descriptorHeap, uint32_t descriptorSize);
D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(DescriptorDSVData& descriptorHeap, uint32_t descriptorSize);

D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(DescriptorSRVData& descriptorHeap, uint32_t descriptorSize);