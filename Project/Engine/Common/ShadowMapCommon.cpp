#include "ShadowMapCommon.h"
#include "WindowsApp.h"

#include "DescriptorHeap.h"
#include "DepthStencil.h"

ShadowMapCommon* ShadowMapCommon::instance_ = nullptr;

ShadowMapCommon *ShadowMapCommon::GetInstance()
{
	if(!instance_){
		instance_ = new ShadowMapCommon();
		instance_->Initialize();
	}

	return instance_;
}

void ShadowMapCommon::Initialize()
{
	dxCommon = DirectXCommon::GetInstance();

	///深度情報
	shadowDSVDescriptorHeapSize_ = dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	shadowDSVDescriptorHeap_.Heap = CreateDescriptorHeap(dxCommon->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);
	//とりあえず1つだけヒープの確保
	shadowDSVDescriptorHeap_.CPUFlags.resize(1);
	shadowDSVDescriptorHeap_.CPUFlags.assign(shadowDSVDescriptorHeap_.CPUFlags.size(), false);
	shadowDSVDescriptorHeap_.GPUFlags.resize(1);
	shadowDSVDescriptorHeap_.GPUFlags.assign(shadowDSVDescriptorHeap_.GPUFlags.size(), false);

	///深度テクスチャ
	shadowTextureSRVDescriptorHeapSize_ = dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	shadowTextureSRVDescriptorHeap_.Heap = CreateDescriptorHeap(dxCommon->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1, true);
	//とりあえず実装するシャドウは1つ
	shadowDSVDescriptorHeap_.CPUFlags.resize(1);
	shadowDSVDescriptorHeap_.CPUFlags.assign(shadowTextureSRVDescriptorHeap_.CPUFlags.size(), false);
	shadowDSVDescriptorHeap_.GPUFlags.resize(1);
	shadowDSVDescriptorHeap_.GPUFlags.assign(shadowTextureSRVDescriptorHeap_.GPUFlags.size(), false);


	///バッファ
	shadowResource_ = CreateDepthStencilTextureResource(dxCommon->GetDevice(), WindowsApp::kWindowWidth_, WindowsApp::kWindowHeight_);

	///バッファビュー
	//深度情報
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandleCPU = GetCPUDescriptorHandle(shadowDSVDescriptorHeap_, shadowDSVDescriptorHeapSize_);
	dxCommon->GetDevice()->CreateDepthStencilView(shadowResource_.Get(), &dsvDesc, dsvHandleCPU);

	//深度テクスチャ
	D3D12_SHADER_RESOURCE_VIEW_DESC viewDesc{};
	viewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	viewDesc.Texture2D.MipLevels = 1;
	viewDesc.Texture2D.MostDetailedMip = 0;
	viewDesc.Texture2D.PlaneSlice = 0;
	viewDesc.Texture2D.ResourceMinLODClamp = 0.0F;
	viewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	D3D12_CPU_DESCRIPTOR_HANDLE textureDsvHandleCPU = GetCPUDescriptorHandle(shadowTextureSRVDescriptorHeap_, shadowDSVDescriptorHeapSize_);
	dxCommon->GetDevice()->CreateDepthStencilView(shadowResource_.Get(), &dsvDesc, dsvHandleCPU);
	dxCommon->GetDevice()->CreateShaderResourceView(shadowResource_.Get(), &viewDesc, textureDsvHandleCPU);
}
