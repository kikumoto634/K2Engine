#include "ShadowCommon.h"
#include <cassert>
#include <d3dx12.h>

void ShadowCommon::Initialize()
{
	HRESULT result = {};
	dxCommon = DirectXCommon::GetInstance();

	//ヒープ
	D3D12_DESCRIPTOR_HEAP_DESC srvDescHeapDesc = {};
	srvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvDescHeapDesc.NumDescriptors = 1;
	srvDescHeapDesc.NodeMask = 0;
	//生成
	result = dxCommon->GetDevice()->CreateDescriptorHeap(
		&srvDescHeapDesc,
		IID_PPV_ARGS(&shadowDSVHeap)
	);
	assert(SUCCEEDED(result));

	//深度バッファ
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Width = 256;	//テクスチャ幅
	resourceDesc.Height = 256;	//テクスチャ高さ
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 0;
	resourceDesc.Format = DXGI_FORMAT_R32_FLOAT;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	result = dxCommon->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), 
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc, 
		D3D12_RESOURCE_STATE_GENERIC_READ, 
		&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0), 
		IID_PPV_ARGS(&shadowResources_)
	);
	assert(SUCCEEDED(result));


	//深度バッファビュー
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	dsvDesc.Texture2D.MipSlice = 0;


	dxCommon->GetDevice()->CreateDepthStencilView(
		shadowResources_.Get(), 
		&dsvDesc,
		shadowDSVHeap->GetCPUDescriptorHandleForHeapStart()
	);

	//パイプライン
	pipeline_ = new ShadowPipeline();
	pipeline_->Create(
		L"Resources/shaders/Object3D.VS.Shadow.hlsl",
		L"Resources/shaders/Object3D.PS.Shadow.hlsl"
	);
}

void ShadowCommon::PreDraw()
{
	dxCommon->GetCommandList()->ResourceBarrier(1, 
		&CD3DX12_RESOURCE_BARRIER::Transition(
			shadowResources_.Get(), 
			D3D12_RESOURCE_STATE_GENERIC_READ, 
			D3D12_RESOURCE_STATE_DEPTH_WRITE)
	);

	//コマンド記録
	dxCommon->GetCommandList()->OMSetRenderTargets(0, nullptr, false, &shadowDSVHeap->GetCPUDescriptorHandleForHeapStart());

	//パイプライン
	dxCommon->GetCommandList()->SetPipelineState(pipeline_->GetGraphicsPipelineState());

	//シャドウマップクリア
	dxCommon->GetCommandList()->ClearDepthStencilView(shadowDSVHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0,0,nullptr);

	CD3DX12_VIEWPORT viewports;
	CD3DX12_RECT scissorRects;
	viewports = CD3DX12_VIEWPORT(0.0F,0.0F,(FLOAT)WindowsApp::kWindowWidth_,(FLOAT)WindowsApp::kWindowHeight_);
	scissorRects = CD3DX12_RECT(0,0,WindowsApp::kWindowWidth_,WindowsApp::kWindowHeight_);
	//ビューポート
	dxCommon->GetCommandList()->RSSetViewports(1,&viewports);
	//シザー
	dxCommon->GetCommandList()->RSSetScissorRects(1,&scissorRects);

}

void ShadowCommon::PostDraw()
{
	dxCommon->GetCommandList()->ResourceBarrier(1, 
		&CD3DX12_RESOURCE_BARRIER::Transition(
			shadowResources_.Get(), 
			D3D12_RESOURCE_STATE_DEPTH_WRITE, 
			D3D12_RESOURCE_STATE_GENERIC_READ)
	);
}
