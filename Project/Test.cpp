#include "Test.h"

#include "Camera.h"
#include "LightingGroup.h"
#include "SpriteLoader.h"

#include "DescriptorHeap.h"
#include "BufferResource.h"
#include "BufferView.h"
#include "GlobalVariables.h"

void Test::Initialize(bool isIndexEnable)
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
	CreateCamera();
}

void Test::Update()
{

}

void Test::Draw(Camera* camera)
{
	for(int i = 0; i < kNumMaxInstance_; i++){
		if(materialData_[i].color != color_ || materialData_[i].shininess != LightingGroup::GetInstance()->GetSpecularPower()){
			materialData_[i].color = color_;
			materialData_[i].shininess = LightingGroup::GetInstance()->GetSpecularPower();
		}

		Matrix4x4 worldViewProjectionMatrix = GetWorldMatrix() * camera->GetViewProjectionMatrix();
		if(wvpData_[i].WVP != worldViewProjectionMatrix || cameraData_[i].worldPosition != camera->translate){
			wvpData_[i].WVP = worldViewProjectionMatrix;
			wvpData_[i].World = GetWorldMatrix();

			cameraData_[i].worldPosition = camera->translate;
		}
	}

	//頂点関連
	dxCommon->GetCommandList()->IASetVertexBuffers(0,1,&vertexBufferView_);		//VBV設定
	if(isIndexDataEnable_)dxCommon->GetCommandList()->IASetIndexBuffer(&indexBufferView_);		//IBV設定

	//RootParams
	dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(
		TestCommon::DESCRIPTOR_PIXEL_TEXTURE, 
		texture_.srvHandleGPU_
	);
	dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(
		TestCommon::CBV_ALL_LIGHT, 
		LightingGroup::GetInstance()->GetResource()->GetGPUVirtualAddress()
	);
	dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(
		TestCommon::DESCRIPTOR_VERTEX_WVP,
		wvpInstancingGPU_
	);
	dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(
		TestCommon::DESCRIPTOR_PIXEL_MATERIAL,
		materialInstancingGPU_
	);
	dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(
		TestCommon::DESCRIPTOR_PIXEL_CAMERA,
		cameraInstancingGPU_
	);

	//描画
	isIndexDataEnable_ ? 
		dxCommon->GetCommandList()->DrawIndexedInstanced(indexNum_,1,0,0,0) : 
		dxCommon->GetCommandList()->DrawInstanced(vertNum_,1,0,0);
}

void Test::CreateVertex()
{
	//リソース
	vertexResource_ = CreateBufferResource(dxCommon->GetDevice(), sizeof(VertexData)*vertNum_);
	//ビュー
	CreateBufferView(vertexBufferView_, vertexResource_.Get(), sizeof(VertexData)*vertNum_, sizeof(VertexData));

	//頂点リソースにデータを書き込む
	//書き込むためのアドレス取得
	vertexResource_->Map(0,nullptr,reinterpret_cast<void**>(&vertData_));
}

void Test::CreateIndex()
{
	if(!isIndexDataEnable_) return ;

	indexResource_ = CreateBufferResource(dxCommon->GetDevice(), sizeof(uint32_t)*indexNum_);

	CreateBufferView(indexBufferView_, indexResource_.Get(), sizeof(uint32_t)*indexNum_);

	//インデックスリソースにデータを書き込む
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
}

void Test::CreateMaterial()
{
	materialResource_ = CreateBufferResource(dxCommon->GetDevice(), sizeof(GeometryMaterial) * kNumMaxInstance_);

	materialResource_->Map(0,nullptr,reinterpret_cast<void**>(&materialData_));
	for(int i = 0; i < kNumMaxInstance_; i++){
		materialData_[i].enableLighting = isLightEnable_;
		materialData_[i].uvTransform = materialData_->uvTransform.MakeIdentityMatrix();
		materialData_[i].shininess = LightingGroup::GetInstance()->GetSpecularPower();
	}

	//SRV作成
	D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc{};
	instancingSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	instancingSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancingSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	instancingSrvDesc.Buffer.FirstElement = 0;
	instancingSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	instancingSrvDesc.Buffer.NumElements = kNumMaxInstance_;
	instancingSrvDesc.Buffer.StructureByteStride = sizeof(GeometryMaterial);

	materialInstancingCPU_ = GetCPUDescriptorHandle(dxCommon->GetSRVDescriptorHeap(), dxCommon->GetDescriptorSizeSRV());
	materialInstancingGPU_ = GetGPUDescriptorHandle(dxCommon->GetSRVDescriptorHeap(), dxCommon->GetDescriptorSizeSRV());
	dxCommon->GetDevice()->CreateShaderResourceView(materialResource_.Get(), &instancingSrvDesc, materialInstancingCPU_);
}

void Test::CreateWVP()
{
	wvpResource_ = CreateBufferResource(dxCommon->GetDevice(), sizeof(TransformationMatrix)*kNumMaxInstance_);

	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
	
	Matrix4x4 worldMatrix = worldMatrix.MakeIdentityMatrix();
	for(int i = 0; i < kNumMaxInstance_; i++){
		wvpData_[i].WVP = worldMatrix;
		wvpData_[i].World = worldMatrix;
	}

	//SRV作成
	D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc{};
	instancingSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	instancingSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancingSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	instancingSrvDesc.Buffer.FirstElement = 0;
	instancingSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	instancingSrvDesc.Buffer.NumElements = kNumMaxInstance_;
	instancingSrvDesc.Buffer.StructureByteStride = sizeof(TransformationMatrix);

	wvpInstancingCPU_ = GetCPUDescriptorHandle(dxCommon->GetSRVDescriptorHeap(), dxCommon->GetDescriptorSizeSRV());
	wvpInstancingGPU_ = GetGPUDescriptorHandle(dxCommon->GetSRVDescriptorHeap(), dxCommon->GetDescriptorSizeSRV());
	dxCommon->GetDevice()->CreateShaderResourceView(wvpResource_.Get(), &instancingSrvDesc, wvpInstancingCPU_);
}

void Test::CreateCamera()
{
	cameraResource_ = CreateBufferResource(dxCommon->GetDevice(), sizeof(CameraForGPUData)*kNumMaxInstance_);

	cameraResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData_));
	
	for(int i = 0; i < kNumMaxInstance_; i++){
		cameraData_[i].worldPosition = {0,0,0};
	}

	//SRV作成
	D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc{};
	instancingSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	instancingSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancingSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	instancingSrvDesc.Buffer.FirstElement = 0;
	instancingSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	instancingSrvDesc.Buffer.NumElements = kNumMaxInstance_;
	instancingSrvDesc.Buffer.StructureByteStride = sizeof(CameraForGPUData);

	cameraInstancingCPU_ = GetCPUDescriptorHandle(dxCommon->GetSRVDescriptorHeap(), dxCommon->GetDescriptorSizeSRV());
	cameraInstancingGPU_ = GetGPUDescriptorHandle(dxCommon->GetSRVDescriptorHeap(), dxCommon->GetDescriptorSizeSRV());
	dxCommon->GetDevice()->CreateShaderResourceView(cameraResource_.Get(), &instancingSrvDesc, cameraInstancingCPU_);
}