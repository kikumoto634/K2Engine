#pragma once
#include <d3d12.h>

/// <summary>
/// 頂点ビュー作成関数
/// </summary>
/// <param name="sizeInByte">使用するバイトサイズ</param>
/// <param name="Num">使用する個数</param>
void CreateBufferView(D3D12_VERTEX_BUFFER_VIEW& view, ID3D12Resource* resource, UINT sizeInByte, UINT strideInBytes);
/// <summary>
/// インデックスビュー作成関数
/// </summary>
/// <param name="sizeInByte">使用するバイトサイズ</param>
/// <param name="Num">使用する個数</param>
void CreateBufferView(D3D12_INDEX_BUFFER_VIEW& view, ID3D12Resource* resource, UINT sizeInByte);

void CreateComputreView(D3D12_UNORDERED_ACCESS_VIEW_DESC& view, UINT numElements, UINT structureByteStride);