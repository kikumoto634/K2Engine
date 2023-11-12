#include "DescriptorHeap.h"
#include <cassert>

ID3D12DescriptorHeap *CreateDescriptorHeap(ID3D12Device *device, D3D12_DESCRIPTOR_HEAP_TYPE heaptype, UINT numDescriptors, bool shaderVisible)
{
    ID3D12DescriptorHeap* descriptorHeap = nullptr;
    D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};

    descriptorHeapDesc.Type = heaptype;
    descriptorHeapDesc.NumDescriptors = numDescriptors;
    descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    HRESULT result = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
    assert(SUCCEEDED(result));

    return descriptorHeap;
}

D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap *descriptorHeap, uint32_t descriptorSize, uint32_t index)
{
    //ディスクリプタの先頭アドレスを取得
    D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
    handleCPU.ptr += (descriptorSize * index);

    return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap *descriptorHeap, uint32_t descriptorSize, uint32_t index)
{
    //ディスクリプタの先頭アドレスを取得
    D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
    handleGPU.ptr += (descriptorSize * index);

    return handleGPU;
}
