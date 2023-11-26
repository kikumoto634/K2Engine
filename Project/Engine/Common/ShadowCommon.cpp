#include "ShadowCommon.h"
#include <cassert>
#include <d3dx12.h>

void ShadowCommon::Initialize()
{
	HRESULT result = {};
	dxCommon = DirectXCommon::GetInstance();

	//深度バッファヒープ
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.NumDescriptors = 1;
	descriptorHeapDesc.Type	= D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	descriptorHeapDesc.NodeMask = 0;

	result = dxCommon->GetDevice()->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&shadowDescriptorHeap_));
	assert(SUCCEEDED(result));


	//深度バッファ
	D3D12_HEAP_PROPERTIES heapProperties{};
	D3D12_RESOURCE_DESC resourceDesc{};
	D3D12_CLEAR_VALUE clearValue{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProperties.CreationNodeMask = 0;
	heapProperties.VisibleNodeMask = 0;

	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Width = WindowsApp::kWindowWidth_;
	resourceDesc.Height = WindowsApp::kWindowHeight_;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 0;
	resourceDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	clearValue.Format = DXGI_FORMAT_D32_FLOAT;
	clearValue.DepthStencil.Depth = 1.0f;
	clearValue.DepthStencil.Stencil = 0;

	result = dxCommon->GetDevice()->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE,
		&resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, &clearValue, IID_PPV_ARGS(&shadowResources_));
	assert(SUCCEEDED(result));


	//深度バッファビュー
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.Texture2D.MipSlice = 0;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dxCommon->GetDevice()->CreateDepthStencilView(shadowResources_.Get(), &dsvDesc, shadowDescriptorHeap_->GetCPUDescriptorHandleForHeapStart());

}

void ShadowCommon::PreDraw()
{
	//リソースバリア書き込み可能
	dxCommon->GetCommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(shadowResources_.Get(),
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE));

	//指定した深度で画面全体をクリア
	dxCommon->GetCommandList()->ClearDepthStencilView(shadowDescriptorHeap_->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0,0, nullptr);

	//Screen設定
	dxCommon->GetCommandList()->RSSetViewports(1,&dxCommon->GetViewport());			//ビューポート
	dxCommon->GetCommandList()->RSSetScissorRects(1,&dxCommon->GetScissorRect());	//シザー矩形

	dxCommon->GetCommandList()->OMSetRenderTargets(0,nullptr,false,&shadowDescriptorHeap_->GetCPUDescriptorHandleForHeapStart());
}

void ShadowCommon::PostDraw()
{
	HRESULT result{};

	//リソースバリア書き込み可能
	dxCommon->GetCommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(shadowResources_.Get(),
		D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ));

	////コマンドリストの内容を確定。
	//result = dxCommon->GetCommandList()->Close();
	//assert(SUCCEEDED(result));

	////GPUにコマンドリストの実行を行わせる
	//ID3D12CommandList* commandList[] = {dxCommon->GetCommandList()};
	//dxCommon->GetCommandQueue()->ExecuteCommandLists(1, commandList);

	////次のフレーム用のコマンドリスト用意
	//result = dxCommon->GetCommandAllocator()->Reset();
	//assert(SUCCEEDED(result));
	//result = dxCommon->GetCommandList()->Reset(dxCommon->GetCommandAllocator(), nullptr);
	//assert(SUCCEEDED(result));
}
