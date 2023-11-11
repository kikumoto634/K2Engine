#pragma once
#include <d3d12.h>
#include <cstdint>

//深度テクスチャの作成
ID3D12Resource* CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height);


