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

D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(DescriptorSRVData& descriptorHeap, uint32_t descriptorSize)
{
    //ディスクリプタの先頭アドレスを取得
    D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap.Heap->GetCPUDescriptorHandleForHeapStart();
    uint32_t index = 0;
    for(int i = 0; i < descriptorHeap.CPUFlags.size(); i++){
        if(descriptorHeap.CPUFlags[i] == false){
            index = i;
            break;
        }
    }

    descriptorHeap.CPUFlags[index] = true;
    handleCPU.ptr += (descriptorSize * index);

    return handleCPU;
}

D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(DescriptorRTVData& descriptorHeap, uint32_t descriptorSize)
{
    //ディスクリプタの先頭アドレスを取得
    D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap.Heap->GetCPUDescriptorHandleForHeapStart();
    uint32_t index = 0;
    for(int i = 0; i < descriptorHeap.CPUFlags.size(); i++){
        if(!descriptorHeap.CPUFlags[i]){
            index = i;
            break;
        }
    }

    descriptorHeap.CPUFlags[index] = true;
    handleCPU.ptr += (descriptorSize * index);

    return handleCPU;
}


D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(DescriptorSRVData& descriptorHeap, uint32_t descriptorSize)
{
    //ディスクリプタの先頭アドレスを取得
    D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap.Heap->GetGPUDescriptorHandleForHeapStart();
    uint32_t index = 0;
    for(int i = 0; i < descriptorHeap.GPUFlags.size(); i++){
        if(!descriptorHeap.GPUFlags[i]){
            index = i;
            break;
        }
    }

    descriptorHeap.GPUFlags[index] = true;
    handleGPU.ptr += (descriptorSize * index);

    return handleGPU;
}
