#include "EmitterFrameBase.h"
#include <BufferResource.h>
#include <BufferView.h>

#include "Particle/EmitterFrame/Common/EmitterFrameCommon.h"

void EmitterFrameBase::Initialize(bool isIndexEnable)
{
	dxCommon = DirectXCommon::GetInstance();

	CreateVertex();
	CreateIndex();
	CreateMaterial();
	CreateWVP();
}

void EmitterFrameBase::Draw(Camera* camera)
{
	Matrix4x4 worldViewProjectionMatrix = GetWorldMatrix() * camera->GetViewProjectionMatrix();
	*wvpData_ = worldViewProjectionMatrix;


	//頂点関連
	dxCommon->GetCommandList()->IASetVertexBuffers(0,1,&vertexBufferView_);		//VBV設定
	dxCommon->GetCommandList()->IASetIndexBuffer(&indexBufferView_);		//IBV設定

	//Pipeline関連
	dxCommon->GetCommandList()->SetGraphicsRootSignature(EmitterFrameCommon::GetInstance()->GetPipeline()->GetRootSignature());
	dxCommon->GetCommandList()->SetPipelineState(EmitterFrameCommon::GetInstance()->GetPipeline()->GetGraphicsPipelineState());	//PSO設定
	dxCommon->GetCommandList()->IASetPrimitiveTopology(EmitterFrameCommon::GetInstance()->GetTopology());


	//RootParams
	dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(
		EmitterFrameCommon::CBV_VERTEX_WVP, 
		wvpResource_->GetGPUVirtualAddress()
	);
	dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(
		EmitterFrameCommon::CBV_PIXEL_Material, 
		constResource_->GetGPUVirtualAddress()
	);

	//描画
	dxCommon->GetCommandList()->DrawIndexedInstanced(indexNum_,1,0,0,0);
}

void EmitterFrameBase::CreateVertex()
{
	//リソース
	vertexResource_ = CreateBufferUploadResource(dxCommon->GetDevice(), sizeof(Vector4)*vertNum_);
	//ビュー
	CreateBufferView(vertexBufferView_, vertexResource_.Get(), sizeof(Vector4)*vertNum_, sizeof(Vector4));
	//頂点リソースにデータを書き込む
	//書き込むためのアドレス取得
	vertexResource_->Map(0,nullptr,reinterpret_cast<void**>(&vertData_));
}

void EmitterFrameBase::CreateIndex()
{
	indexResource_ = CreateBufferUploadResource(dxCommon->GetDevice(), sizeof(uint32_t)*indexNum_);

	CreateBufferView(indexBufferView_, indexResource_.Get(), sizeof(uint32_t)*indexNum_);

	//インデックスリソースにデータを書き込む
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
}

void EmitterFrameBase::CreateMaterial()
{
	constResource_ = CreateBufferUploadResource(dxCommon->GetDevice(), sizeof(Vector4));

	constResource_->Map(0,nullptr,reinterpret_cast<void**>(&materialData_));
	*materialData_ = color_;
}

void EmitterFrameBase::CreateWVP()
{
	wvpResource_ = CreateBufferUploadResource(dxCommon->GetDevice(), sizeof(Matrix4x4));

	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
	
	Matrix4x4 worldMatrix = worldMatrix.MakeIdentityMatrix();
	*wvpData_ = worldMatrix;
}
