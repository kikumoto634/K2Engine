#include "GeometryBase.h"

#include "Camera.h"
#include "LightingGroup.h"
#include "SpriteLoader.h"

#include "DescriptorHeap.h"
#include "BufferResource.h"
#include "BufferView.h"
#include "GlobalVariables.h"

void GeometryBase::Initialize(bool isIndexEnable)
{
	dxCommon = DirectXCommon::GetInstance();

	name = typeid(*this).name();

	isIndexDataEnable_ = isIndexEnable;
	texture_ = SpriteLoader::SearchTexture(texturePath_);

	//リソース
	CreateVertex();
	CreateIndex();
	CreateMaterial();
	CreateWVP();

	isPipelineCreateCheck = PipelineCreate();
}

void GeometryBase::Update()
{
	if(collider_)collider_->Update();
}

void GeometryBase::Draw(Camera* camera)
{
	materialData_->color = color_;
	Matrix4x4 worldViewProjectionMatrix = GetWorldMatrix() * camera->GetViewProjectionMatrix();
	wvpData_->WVP = worldViewProjectionMatrix;
	wvpData_->World = worldViewProjectionMatrix;

	//頂点関連
	dxCommon->GetCommandList()->IASetVertexBuffers(0,1,&vertexBufferView_);		//VBV設定
	if(isIndexDataEnable_)dxCommon->GetCommandList()->IASetIndexBuffer(&indexBufferView_);		//IBV設定

	//Pipeline関連
	dxCommon->GetCommandList()->SetGraphicsRootSignature(
		isPipelineCreateCheck ? 
		pipelineDatas.pipeline_->GetRootSignature() :
		GeometryCommon::GetInstance()->GetPipeline()->GetRootSignature()
	);
	dxCommon->GetCommandList()->SetPipelineState(
		isPipelineCreateCheck ? 
		pipelineDatas.pipeline_->GetGraphicsPipelineState() :
		GeometryCommon::GetInstance()->GetPipeline()->GetGraphicsPipelineState()
	);
	dxCommon->GetCommandList()->IASetPrimitiveTopology(
		isPipelineCreateCheck ? 
		pipelineDatas.commandPrimitiveTopology_ :
		GeometryCommon::GetInstance()->GetTopology()
	);

	//RootParams
	dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(
		GeometryCommon::DESCRIPTOR_PIXEL_TEXTURE, 
		texture_.srvHandleGPU_
	);
	dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(
		GeometryCommon::CBV_ALL_LIGHT, 
		LightingGroup::GetInstance()->GetResource()->GetGPUVirtualAddress()
	);
	dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(
		GeometryCommon::CBV_VERTEX_WVP,
		wvpResource_->GetGPUVirtualAddress()
	);
	dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(
		GeometryCommon::CBV_PIXEL_MATERIAL,
		materialResource_->GetGPUVirtualAddress()
	);

	//描画
	isIndexDataEnable_ ? 
		dxCommon->GetCommandList()->DrawIndexedInstanced(indexNum_,1,0,0,0) : 
		dxCommon->GetCommandList()->DrawInstanced(vertNum_,1,0,0);

	if(collider_) collider_->ShapeDraw(camera);
}

void GeometryBase::CreateVertex()
{
	//リソース
	vertexResource_ = CreateBufferResource(dxCommon->GetDevice(), sizeof(VertexData)*vertNum_);
	//ビュー
	CreateBufferView(vertexBufferView_, vertexResource_.Get(), sizeof(VertexData)*vertNum_, sizeof(VertexData));

	//頂点リソースにデータを書き込む
	//書き込むためのアドレス取得
	vertexResource_->Map(0,nullptr,reinterpret_cast<void**>(&vertData_));
}

void GeometryBase::CreateIndex()
{
	if(!isIndexDataEnable_) return ;

	indexResource_ = CreateBufferResource(dxCommon->GetDevice(), sizeof(uint32_t)*indexNum_);

	CreateBufferView(indexBufferView_, indexResource_.Get(), sizeof(uint32_t)*indexNum_);

	//インデックスリソースにデータを書き込む
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
}

void GeometryBase::CreateMaterial()
{
	materialResource_ = CreateBufferResource(dxCommon->GetDevice(), sizeof(GeometryMaterial));

	materialResource_->Map(0,nullptr,reinterpret_cast<void**>(&materialData_));
	materialData_->enableLighting = isLightEnable_;
	materialData_->uvTransform = materialData_->uvTransform.MakeIdentityMatrix();
}

void GeometryBase::CreateWVP()
{
	wvpResource_ = CreateBufferResource(dxCommon->GetDevice(), sizeof(TransformationMatrix));

	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
	
	Matrix4x4 worldMatrix = worldMatrix.MakeIdentityMatrix();
	wvpData_->WVP = worldMatrix;
	wvpData_->World = worldMatrix;
}

void GeometryBase::ApplyGlobalVariablesInitialize()
{
#ifdef _DEBUG
	//GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	////グループを追加
	//GlobalVariables::GetInstance()->CreateGroup(name_);
	//globalVariables->AddItem(name_, "0.translate", translate);
	//globalVariables->AddItem(name_, "1.rotate", rotation);
	//globalVariables->AddItem(name_, "2.scale", scale);
#endif // _DEBUG
}

void GeometryBase::ApplyGlobalVariablesUpdate()
{
#ifdef _DEBUG
	/*GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	translate = globalVariables->GetVector3Value(name_, "0.translate");
	rotation = globalVariables->GetVector3Value(name_, "1.rotate");
	scale = globalVariables->GetVector3Value(name_, "2.scale");*/
#endif // _DEBUG
}
