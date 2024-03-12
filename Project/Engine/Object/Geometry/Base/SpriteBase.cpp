#include "SpriteBase.h"

#include "Camera.h"
#include "SpriteLoader.h"

#include "DescriptorHeap.h"
#include "BufferResource.h"
#include "BufferView.h"
#include "GlobalVariables.h"

void SpriteBase::Initialize()
{
	dxCommon = DirectXCommon::GetInstance();

	texture_ = SpriteLoader::SearchTexture(texturePath_);
	textureSize_ = texture_.size;

	//リソース
	CreateVertex();
	CreateIndex();
	CreateMaterial();
	CreateWVP();

	isPipelineCreateCheck = PipelineCreate();
}

void SpriteBase::Draw(Camera* camera)
{
	//計算
	Matrix4x4 viewProjMatrix = MakeIdentityMatrix();
	Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(0.0f,0.0f, (float)WindowsApp::kWindowWidth_,(float)WindowsApp::kWindowHeight_, 0.0f,100.0f);
	viewProjMatrix = projectionMatrixSprite;
	Matrix4x4 worldViewProjectionMatrixSprite = GetWorldMatrix() * viewProjMatrix;
	wvpData_->WVP = worldViewProjectionMatrixSprite;
	wvpData_->World = worldViewProjectionMatrixSprite;
	//更新
	materialData_->uvTransform= MakeAffineMatrix(uvTransformSprite_.scale,uvTransformSprite_.rotation,uvTransformSprite_.translate);


	//頂点関連
	dxCommon->GetCommandList()->IASetVertexBuffers(0,1,&vertexBufferView_);		//VBV設定
	dxCommon->GetCommandList()->IASetIndexBuffer(&indexBufferView_);		//IBV設定
	
	//Pipeline関連
	dxCommon->GetCommandList()->SetGraphicsRootSignature(
		isPipelineCreateCheck ? 
		pipelineDatas.pipeline_->GetRootSignature() :
		SpriteCommon::GetInstance()->GetPipeline()->GetRootSignature()
	);
	dxCommon->GetCommandList()->SetPipelineState(
		isPipelineCreateCheck ? 
		pipelineDatas.pipeline_->GetGraphicsPipelineState() :
		SpriteCommon::GetInstance()->GetPipeline()->GetGraphicsPipelineState()
	);
	dxCommon->GetCommandList()->IASetPrimitiveTopology(
		isPipelineCreateCheck ? 
		pipelineDatas.commandPrimitiveTopology_ :
		SpriteCommon::GetInstance()->GetTopology()
	);

	//RootParams設定
	dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(
		SpriteCommon::CBV_PIXEL_MATERIAL, 
		constResource_->GetGPUVirtualAddress()
	);
	dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(
		SpriteCommon::CBV_VERTEX_WVP, 
		wvpResource_->GetGPUVirtualAddress()
	);
	dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(
		SpriteCommon::DESCRIPTOR_PIXEL_TEXTURE,
		texture_.srvHandleGPU_
	);

	//描画
	dxCommon->GetCommandList()->DrawIndexedInstanced(indexNum_,1,0,0,0);
}


void SpriteBase::CreateVertex()
{
	vertexResource_ = CreateBufferUploadResource(dxCommon->GetDevice() ,sizeof(VertexData)*vertNum_);
	CreateBufferView(vertexBufferView_, vertexResource_.Get(), sizeof(VertexData)*vertNum_, sizeof(VertexData));
	vertexResource_->Map(0,nullptr,reinterpret_cast<void**>(&vertData_));
}

void SpriteBase::CreateIndex()
{
	indexResource_ = CreateBufferUploadResource(dxCommon->GetDevice() ,sizeof(uint32_t)*indexNum_);
	CreateBufferView(indexBufferView_, indexResource_.Get(), sizeof(uint32_t)*indexNum_);
	//インデックスリソースにデータを書き込む
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
}

void SpriteBase::CreateMaterial()
{
	constResource_ = CreateBufferUploadResource(dxCommon->GetDevice() ,sizeof(TextureMaterial));

	constResource_->Map(0,nullptr,reinterpret_cast<void**>(&materialData_));
	materialData_->color = color_;
	materialData_->uvTransform =materialData_->uvTransform.MakeIdentityMatrix();
}

void SpriteBase::CreateWVP()
{
	wvpResource_ = CreateBufferUploadResource(dxCommon->GetDevice() ,sizeof(TransformationMatrix));
	wvpResource_->Map(0,nullptr,reinterpret_cast<void**>(&wvpData_));
	Matrix4x4 worldMatrixSprite = worldMatrixSprite.MakeIdentityMatrix();
	wvpData_->WVP = worldMatrixSprite;
	wvpData_->World = worldMatrixSprite;
}


void SpriteBase::ApplyGlobalVariablesInitialize()
{
#ifdef _DEBUG
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	//グループを追加
	GlobalVariables::GetInstance()->CreateGroup(texture_.name);
	globalVariables->AddItem(texture_.name, "0.translate", translate);
	globalVariables->AddItem(texture_.name, "1.rotate", rotation);
	globalVariables->AddItem(texture_.name, "2.scale", scale);
#endif // _DEBUG
}

void SpriteBase::ApplyGlobalVariablesUpdate()
{
#ifdef _DEBUG
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	translate = globalVariables->GetVector3Value(texture_.name, "0.translate");
	rotation = globalVariables->GetVector3Value(texture_.name, "1.rotate");
	scale = globalVariables->GetVector3Value(texture_.name, "2.scale");
#endif // _DEBUG
}
