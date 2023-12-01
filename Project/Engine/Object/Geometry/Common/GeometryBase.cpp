#include "GeometryBase.h"

#include "SpriteLoader.h"
#include "DescriptorHeap.h"

#include "BufferResource.h"
#include "BufferView.h"

#include "LightingGroup.h"

std::unique_ptr<GeometryBaseCommon> GeometryBase::common;

void GeometryBase::StaticInitialize()
{
	common = std::make_unique<GeometryBaseCommon>();
	common->Initialize();
}

void GeometryBase::StaticDraw()
{
	//ルートシグネチャ設定 PSOに設定しいているが別途設定が必要
	common->dxCommon->GetCommandList()->SetGraphicsRootSignature(common->pipeline_->GetRootSignature());
	common->dxCommon->GetCommandList()->SetPipelineState(common->pipeline_->GetGraphicsPipelineState());	//PSO設定

	//形状設定、PSOに設定しているのとは別
	common->dxCommon->GetCommandList()->IASetPrimitiveTopology(common->commandPrimitiveTopology);
}

void GeometryBase::Initialize(bool isIndexEnable)
{
	isIndexDataEnable_ = isIndexEnable;
	texture_ = SpriteLoader::SearchTexture(texturePath_);

	//リソース
	CreateVertex();
	CreateIndex();
	CreateMaterial();
	CreateWVP();
}

void GeometryBase::Draw(Matrix4x4 viewProjectionMatrix)
{
	Matrix4x4 worldViewProjectionMatrix = transform.GetWorldMatrix() * viewProjectionMatrix;
	wvpData_->WVP = worldViewProjectionMatrix;
	wvpData_->World = worldViewProjectionMatrix;

	common->dxCommon->GetCommandList()->IASetVertexBuffers(0,1,&vertexBufferView_);		//VBV設定
	if(isIndexDataEnable_)common->dxCommon->GetCommandList()->IASetIndexBuffer(&indexBufferView_);		//IBV設定

	//SRV(テクスチャ)のDescriptorTableの先頭を設定 2はRootParamterのインデックスRootParamter[2]
	common->dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(0, texture_.srvHandleGPU_);
	//Light
	common->dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, LightingGroup::GetInstance()->GetResource()->GetGPUVirtualAddress());
	//行列のwvpBufferの場所を設定 ※RootParameter[1]に対してCBVの設定
	common->dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(2, wvpResource_->GetGPUVirtualAddress());
	//マテリアルのconstBufferの場所を設定
	common->dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(3, constResource_->GetGPUVirtualAddress());

	//描画
	isIndexDataEnable_ ? 
		common->dxCommon->GetCommandList()->DrawIndexedInstanced(indexNum_,1,0,0,0) : 
		common->dxCommon->GetCommandList()->DrawInstanced(vertNum_,1,0,0);
}


void GeometryBase::CreateVertex()
{
	//リソース
	vertexResource_ = CreateBufferResource(common->dxCommon->GetDevice(), sizeof(VertexData)*vertNum_);
	//ビュー
	CreateBufferView(vertexBufferView_, vertexResource_.Get(), sizeof(VertexData)*vertNum_, sizeof(VertexData));

	//頂点リソースにデータを書き込む
	//書き込むためのアドレス取得
	vertexResource_->Map(0,nullptr,reinterpret_cast<void**>(&vertData_));
}

void GeometryBase::CreateIndex()
{
	if(!isIndexDataEnable_) return ;

	indexResource_ = CreateBufferResource(common->dxCommon->GetDevice(), sizeof(uint32_t)*indexNum_);

	CreateBufferView(indexBufferView_, indexResource_.Get(), sizeof(uint32_t)*indexNum_);

	//インデックスリソースにデータを書き込む
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
}

void GeometryBase::CreateMaterial()
{
	constResource_ = CreateBufferResource(common->dxCommon->GetDevice(), sizeof(GeometryMaterial));

	constResource_->Map(0,nullptr,reinterpret_cast<void**>(&materialData_));
	materialData_->color = color_;
	materialData_->enableLighting = isLightEnable;
	materialData_->uvTransform = materialData_->uvTransform.MakeIdentityMatrix();
}

void GeometryBase::CreateWVP()
{
	wvpResource_ = CreateBufferResource(common->dxCommon->GetDevice(), sizeof(TransformationMatrix));

	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
	
	Matrix4x4 worldMatrix = worldMatrix.MakeIdentityMatrix();
	wvpData_->WVP = worldMatrix;
	wvpData_->World = worldMatrix;
}