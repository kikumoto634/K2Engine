#include "GPUParticleBase.h"

#include <random>
#include <d3dx12.h>

#include "Camera.h"

#include "Particle/Common/GPUParticleCommon.h"
#include "BufferResource.h"
#include "BufferView.h"
#include "DescriptorHeap.h"

using namespace Microsoft::WRL;


GPUParticleBase *GPUParticleBase::Create()
{
	GPUParticleBase* instance = new GPUParticleBase();
	instance->Initialize();
	return instance;
}

void GPUParticleBase::Initialize(bool isIndexEnable)
{
	dxCommon = DirectXCommon::GetInstance();
	compute = ComputeCommon::GetInstance();

	CreateVertex();
	CreateMaterial();
	CreateWVP();
	CreateCompute();

	for(int i = 0; i < kNumMaxInstance; i++){
		Transform temp1;
		transfrom_.push_back(temp1);
	}

}

void GPUParticleBase::Draw(Camera* camera)
{
	compute->Excution(kNumMaxInstance);

	int numInstance = 0;	//描画すべきインスタンス
	for(list<Transform>::iterator particleIterator = transfrom_.begin(); particleIterator != transfrom_.end();){
		if(numInstance < kNumMaxInstance){
			
			particleIterator->translate = computeData_[numInstance].position;

			Matrix4x4 worldViewProjectionMatrix = particleIterator->GetWorldMatrix() * camera->GetViewProjectionMatrix();
			wvpData_[numInstance] = worldViewProjectionMatrix;

			++numInstance;
		}
		++particleIterator;
	}

	//頂点関連
	dxCommon->GetCommandList()->IASetVertexBuffers(0,1,&vertexBufferView_);		//VBV設定

	//Pipeline関連
	dxCommon->GetCommandList()->SetGraphicsRootSignature(
		GPUParticleCommon::GetInstance()->GetPipeline()->GetRootSignature()
	);
	dxCommon->GetCommandList()->SetPipelineState(
		GPUParticleCommon::GetInstance()->GetPipeline()->GetGraphicsPipelineState()
	);
	dxCommon->GetCommandList()->IASetPrimitiveTopology(
		GPUParticleCommon::GetInstance()->GetTopology()
	);

	//RootParams
	dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(
		GPUParticleCommon::DESCRIPTOR_VERTEX_WVP, 
		instancingSrvHandleGPU_
	);
	dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(
		GPUParticleCommon::CBV_PIXEL_MATERIAL, 
		constResource_->GetGPUVirtualAddress()
	);

	//描画
	dxCommon->GetCommandList()->DrawInstanced(vertNum_,numInstance,0,0);
}


void GPUParticleBase::CreateVertex()
{
	//中身設置
	vertData_[0] = {0,0,0,1};

	//リソース
	vertexResource_ = CreateBufferDefaultResource(dxCommon->GetDevice(), sizeof(Vector4)*vertNum_);
	//中間リソース
	ComPtr<ID3D12Resource> vertexResourceUpload = CreateBufferUploadResource(dxCommon->GetDevice(), sizeof(Vector4)*vertNum_);

	//この時点で頂点情報を決めないとあかん
	D3D12_SUBRESOURCE_DATA vertexSubResourceData_;
	vertexSubResourceData_.pData = reinterpret_cast<UINT8*>(vertData_);
	vertexSubResourceData_.RowPitch = sizeof(Vector4)*vertNum_;
	vertexSubResourceData_.SlicePitch = vertexSubResourceData_.RowPitch;

	//中間リソース内容をリソースに転送
	UpdateSubresources<1>(
		dxCommon->GetCommandList(), 
		vertexResource_.Get(), vertexResourceUpload.Get(), 
		0, 0, 1, &vertexSubResourceData_
	);

	//コマンド積み
	dxCommon->GetCommandList()->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(vertexResource_.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST,D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER)
	);

	//ビュー
	CreateBufferView(vertexBufferView_, vertexResource_.Get(), sizeof(Vector4)*vertNum_, sizeof(Vector4));
}

void GPUParticleBase::CreateMaterial()
{
	//中身設置
	materialData_[0] = color_;
	
	//リソース
	constResource_ = CreateBufferUploadResource(dxCommon->GetDevice(), sizeof(Vector4));
	//中間リソース
	ComPtr<ID3D12Resource> constResourceUpload = CreateBufferUploadResource(dxCommon->GetDevice(), sizeof(Vector4));

	//この時点で頂点情報を決めないとあかん
	D3D12_SUBRESOURCE_DATA constSubResourceData_;
	constSubResourceData_.pData = reinterpret_cast<UINT8*>(materialData_);
	constSubResourceData_.RowPitch = sizeof(Vector4);
	constSubResourceData_.SlicePitch = constSubResourceData_.RowPitch;

	//中間リソース内容をリソースに転送
	UpdateSubresources<1>(
		dxCommon->GetCommandList(), 
		constResource_.Get(), constResourceUpload.Get(), 
		0, 0, 1, &constSubResourceData_
	);

	//コマンド積み
	dxCommon->GetCommandList()->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(vertexResource_.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST,D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER)
	);
}

void GPUParticleBase::CreateWVP()
{
	wvpResource_ = CreateBufferUploadResource(dxCommon->GetDevice(), sizeof(Matrix4x4)*kNumMaxInstance);

	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
	
	for(int i = 0; i < kNumMaxInstance; i++){
		wvpData_[i] = MakeIdentityMatrix();
	}

	//SRV作成
	D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc{};
	instancingSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	instancingSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancingSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	instancingSrvDesc.Buffer.FirstElement = 0;
	instancingSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	instancingSrvDesc.Buffer.NumElements = kNumMaxInstance;
	instancingSrvDesc.Buffer.StructureByteStride = sizeof(Matrix4x4);

	instancingSrvHandleCPU_ = GetCPUDescriptorHandle(dxCommon->GetSRVDescriptorHeap(), dxCommon->GetDescriptorSizeSRV());
	instancingSrvHandleGPU_ = GetGPUDescriptorHandle(dxCommon->GetSRVDescriptorHeap(), dxCommon->GetDescriptorSizeSRV());
	dxCommon->GetDevice()->CreateShaderResourceView(wvpResource_.Get(), &instancingSrvDesc, instancingSrvHandleCPU_);
}

void GPUParticleBase::CreateCompute()
{
	//リソース
	computeResource_ = CreateComputeBufferResource(
		dxCommon->GetDevice(),
		(sizeof(ComputeData) * kNumMaxInstance + 0xff)&~0xff
	);

	//ビュー
	D3D12_UNORDERED_ACCESS_VIEW_DESC desc{};
	CreateComputreView(desc, (UINT)kNumMaxInstance, sizeof(ComputeData));

	//ここでResourceとコンピュートシェーダに渡すヒープを紐図ける
	dxCommon->GetDevice()->CreateUnorderedAccessView(
		computeResource_.Get(),
		nullptr,
		&desc,
		compute->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart()
	);

	computeResource_->Map(0,nullptr, reinterpret_cast<void**>(&computeData_));

	//乱数生成器
	std::random_device seedGenerator_;
	std::mt19937 randomEngine(seedGenerator_());
	std::uniform_real_distribution<float> distValue(-3,3);
	std::uniform_real_distribution<float> distVelo(-1,1);
	for(int i = 0; i < kNumMaxInstance; i++){

		computeData_[i].position = {distValue(randomEngine), distValue(randomEngine) + 5, distValue(randomEngine)};
		computeData_[i].velocity = {distVelo(randomEngine),distVelo(randomEngine),distVelo(randomEngine)};
	}
}