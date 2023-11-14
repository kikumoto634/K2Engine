#pragma once
#include <d3d12.h>

//ResourceとHeap
// Resourceはメモリ上で配置されるが、「どこ」のメモリに配置するのかは重要
// ・GPUでよく使用するものはGPUに近いほど良い
// ・CPUでよく使用するものはCPUに近いほど良い
	
// これらを全体的に管理するのが、「Heap」 
// Resourceはどの種類のHeap上に配置するのか決めてから作成する
// ・Default	(デフォルト)
// ・Upload		(アップロード)
// ・Readback	(リードバック)

/// <summary>
/// リソース作成関数
/// </summary>
/// <param name="sizeInByte"> 使用するバイトサイズ </param>
ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInByte);