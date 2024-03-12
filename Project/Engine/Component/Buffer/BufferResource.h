#pragma once
#include <d3d12.h>

//ResourceとHeap
// Resourceはメモリ上で配置されるが、「どこ」のメモリに配置するのかは重要
// ・GPUでよく使用するものはGPUに近いほど良い
// ・CPUでよく使用するものはCPUに近いほど良い
	
// これらを全体的に管理するのが、「Heap」 
// Resourceはどの種類のHeap上に配置するのか決めてから作成する
// ・Default	(デフォルト)	GPU
// ・Upload		(アップロード)	CPU
// ・Readback	(リードバック)

/// <summary>
/// リソース作成関数
/// </summary>
/// <param name="sizeInByte"> 使用するバイトサイズ </param>
ID3D12Resource* CreateBufferUploadResource(ID3D12Device* device, size_t sizeInByte);
ID3D12Resource* CreateBufferDefaultResource(ID3D12Device* device, size_t sizeInByte);


ID3D12Resource* CreateComputeBufferResource(ID3D12Device* device, size_t sizeInByte, D3D12_HEAP_TYPE heapType = D3D12_HEAP_TYPE_CUSTOM);