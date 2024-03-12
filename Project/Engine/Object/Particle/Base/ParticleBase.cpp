#include "ParticleBase.h"

#include "Camera.h"

#include "SpriteLoader.h"
#include "BufferResource.h"
#include "BufferView.h"
#include "GlobalVariables.h"
#include "DescriptorHeap.h"

#include "Particle/Common/ParticleCommon.h"

using namespace std;

void ParticleBase::Initialize(bool isIndexEnable)
{
	dxCommon = DirectXCommon::GetInstance();

	isIndexDataEnable_ = isIndexEnable;
	texture_ = SpriteLoader::SearchTexture(texturePath_);

	//リソース
	CreateVertex();
	CreateIndex();
	CreateWVP();

	billboardMatrix_ = MakeIdentityMatrix();
}

void ParticleBase::Update()
{
	for(list<ParticleData>::iterator particleIterator = particles_.begin(); particleIterator != particles_.end();){
		if(particleIterator->lifeTime <= particleIterator->currentTime) {
			particleIterator = particles_.erase(particleIterator);
			continue;
		}

		particleIterator->transform.translate += particleIterator->velocity * kDeltaTime_;
		particleIterator->transform.rotation.z += particleIterator->velocity.z * kDeltaTime_;
		particleIterator->currentTime += kDeltaTime_;

		++particleIterator;
	}
}

void ParticleBase::Draw(Camera* camera)
{
	int numInstance = 0;	//描画すべきインスタンス
	for(list<ParticleData>::iterator particleIterator = particles_.begin(); particleIterator != particles_.end();){

		Matrix4x4 worldViewProjectionMatrix = MakeIdentityMatrix();
		//ビルボードなし
		if(billboardTypeEnable[0]){
			worldViewProjectionMatrix = particleIterator->transform.GetWorldMatrix() * camera->GetViewProjectionMatrix();
		}
		//全方位ビルボード
		else if(billboardTypeEnable[1]){
			billboardMatrix_ = camera->GetWorldMatrix();
			billboardMatrix_.m[3][0] = 0.0f;
			billboardMatrix_.m[3][1] = 0.0f;
			billboardMatrix_.m[3][2] = 0.0f;
			
			Matrix4x4 worldMatrix = MakeScaleMatrix(particleIterator->transform.scale) * billboardMatrix_ * MakeTranslateMatrix(particleIterator->transform.translate);
			worldViewProjectionMatrix = worldMatrix * camera->GetViewProjectionMatrix();
		}
		//Y軸ビルボード
		else if(billboardTypeEnable[2]){
			Matrix4x4 cameraMat = MakeAffineMatrix(camera->scale, {0,camera->rotation.y,camera->rotation.z}, camera->translate);

			billboardMatrix_ = cameraMat;
			billboardMatrix_.m[3][0] = 0.0f;
			billboardMatrix_.m[3][1] = 0.0f;
			billboardMatrix_.m[3][2] = 0.0f;
			
			Matrix4x4 worldMatrix = MakeScaleMatrix(particleIterator->transform.scale) * billboardMatrix_ * MakeTranslateMatrix(particleIterator->transform.translate);
			worldViewProjectionMatrix = worldMatrix * camera->GetViewProjectionMatrix();
		}

		//最大数制御
		if(numInstance < kNumMaxInstance_){
			particleData_[numInstance].WVP = worldViewProjectionMatrix;
			particleData_[numInstance].World = worldViewProjectionMatrix;

			float alpha = 1.0f - (particleIterator->currentTime / particleIterator->lifeTime);
			particleData_[numInstance].color = particleIterator->color;
			particleData_[numInstance].color.w = alpha;

			++numInstance;
		}
		++particleIterator;
	}

	//頂点関連
	dxCommon->GetCommandList()->IASetVertexBuffers(0,1,&vertexBufferView_);		//VBV設定
	if(isIndexDataEnable_)dxCommon->GetCommandList()->IASetIndexBuffer(&indexBufferView_);		//IBV設定

	//Pipeline関連
	dxCommon->GetCommandList()->SetGraphicsRootSignature(ParticleCommon::GetInstance()->GetPipeline()->GetRootSignature());
	dxCommon->GetCommandList()->SetPipelineState(ParticleCommon::GetInstance()->GetPipeline()->GetGraphicsPipelineState());	//PSO設定
	dxCommon->GetCommandList()->IASetPrimitiveTopology(ParticleCommon::GetInstance()->GetTopology());

	//RootParams
	dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(
		ParticleCommon::DESCRIPTOR_PIXEL_TEXTURE, 
		texture_.srvHandleGPU_
	);
	dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(
		ParticleCommon::DESCRIPTOR_VERTEX_WVP,
		instancingSrvHandleGPU_
	);

	//描画
	isIndexDataEnable_ ? 
		dxCommon->GetCommandList()->DrawIndexedInstanced(indexNum_,numInstance,0,0,0) : 
		dxCommon->GetCommandList()->DrawInstanced(vertNum_,numInstance,0,0);
}

void ParticleBase::Add(const Vector3 &translate)
{
	//乱数生成器
	std::random_device seedGenerator_;
	std::mt19937 randomEngine(seedGenerator_());
	particles_.push_back(MakeNewParticle(randomEngine, translate));
}

ParticleData ParticleBase::MakeNewParticle(std::mt19937 &randomEngine, const Vector3& translate)
{
	std::uniform_real_distribution<float> distVelocity(0.0f, 1.0f);
	std::uniform_real_distribution<float> distColor(0.0f,1.0f);
	std::uniform_real_distribution<float> distTime(1.0f,3.0f);
	ParticleData particle;

	particle.transform.scale = {1,1,1};
	particle.transform.rotation = {0,0,0};
	particle.transform.translate = translate;

	particle.velocity = {distVelocity(randomEngine),distVelocity(randomEngine),distVelocity(randomEngine)};

	particle.color = {distColor(randomEngine),distColor(randomEngine),distColor(randomEngine), 1.0f};
	
	particle.lifeTime = distTime(randomEngine);
	particle.currentTime = 0;

	return particle;
}

void ParticleBase::CreateVertex()
{
	//リソース
	vertexResource_ = CreateBufferUploadResource(dxCommon->GetDevice(), sizeof(VertexData)*vertNum_);
	//ビュー
	CreateBufferView(vertexBufferView_, vertexResource_.Get(), sizeof(VertexData)*vertNum_, sizeof(VertexData));

	//頂点リソースにデータを書き込む
	vertexResource_->Map(0,nullptr,reinterpret_cast<void**>(&vertData_));
}

void ParticleBase::CreateIndex()
{
	if(!isIndexDataEnable_) return ;

	indexResource_ = CreateBufferUploadResource(dxCommon->GetDevice(), sizeof(uint32_t)*indexNum_);

	CreateBufferView(indexBufferView_, indexResource_.Get(), sizeof(uint32_t)*indexNum_);

	//インデックスリソースにデータを書き込む
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
}
	
void ParticleBase::CreateWVP()
{
	particleResource_ = CreateBufferUploadResource(dxCommon->GetDevice(), sizeof(ParticleForGPUData)*kNumMaxInstance_);
	particleResource_->Map(0, nullptr, reinterpret_cast<void**>(&particleData_));
	
	for(int i = 0; i < kNumMaxInstance_; i++){
		particleData_[i].WVP = MakeIdentityMatrix();
		particleData_[i].World = MakeIdentityMatrix();
		particleData_[i].color = {1,1,1,1};
	}

	//SRV作成
	D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc{};
	instancingSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	instancingSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancingSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	instancingSrvDesc.Buffer.FirstElement = 0;
	instancingSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	instancingSrvDesc.Buffer.NumElements = kNumMaxInstance_;
	instancingSrvDesc.Buffer.StructureByteStride = sizeof(ParticleForGPUData);

	instancingSrvHandleCPU_ = GetCPUDescriptorHandle(dxCommon->GetSRVDescriptorHeap(), dxCommon->GetDescriptorSizeSRV());
	instancingSrvHandleGPU_ = GetGPUDescriptorHandle(dxCommon->GetSRVDescriptorHeap(), dxCommon->GetDescriptorSizeSRV());
	dxCommon->GetDevice()->CreateShaderResourceView(particleResource_.Get(), &instancingSrvDesc, instancingSrvHandleCPU_);
}


void ParticleBase::ApplyGlobalVariablesInitialize()
{
#ifdef _DEBUG
	//GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	////グループを追加
	//GlobalVariables::GetInstance()->CreateGroup(name_);
	//globalVariables->AddItem(name_, "0.translate", baseParticle_.transform.translate);
	//globalVariables->AddItem(name_, "1.rotate", baseParticle_.transform.rotation);
	//globalVariables->AddItem(name_, "2.scale", baseParticle_.transform.scale);
#endif // _DEBUG
}

void ParticleBase::ApplyGlobalVariablesUpdate()
{
#ifdef _DEBUG
	/*GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	baseParticle_.transform.translate = globalVariables->GetVector3Value(name_, "0.translate");
	baseParticle_.transform.rotation = globalVariables->GetVector3Value(name_, "1.rotate");
	baseParticle_.transform.scale = globalVariables->GetVector3Value(name_, "2.scale");*/
#endif // _DEBUG
}
