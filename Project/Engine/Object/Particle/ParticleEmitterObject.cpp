#include "ParticleEmitterObject.h"
#include <imgui.h>

ParticleEmitterObject *ParticleEmitterObject::Create()
{
	ParticleEmitterObject* instance = new ParticleEmitterObject();
	instance->Initialize();
	instance->VertexData();
	instance->IndexData();
	return instance;
}

//void ParticleEmitterObject::Initialize(bool isIndexEnable)
//{
//	ParticleBase::Initialize(isIndexEnable);
//
//	emitter_.velocity = {-1,1};
//	emitter_.count = 3;
//	emitter_.frequency = 0.5f;
//	emitter_.frequencyTime = 0.0f;
//}

void ParticleEmitterObject::Update()
{
#ifdef _DEBUG
	//ImGui::Text("Emitter");
	/*for(int i = 0; i < 3; i++){
		if(i > 0){ImGui::SameLine();}
		ImGui::Checkbox('EbillboardTypeName[i], &billboardTypeEnable[i]);
	}*/
	if(!particles_.empty())ImGui::DragFloat3("Emitter Pos", &particles_.front().transform.translate.x);
#endif // _DEBUG

	for(std::list<ParticleData>::iterator particleIterator = particles_.begin(); particleIterator != particles_.end();){
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

void ParticleEmitterObject::Draw(Camera *camera)
{
	//int numInstance = 0;	//描画すべきインスタンス
	//for(list<ParticleData>::iterator particleIterator = particles_.begin(); particleIterator != particles_.end();){

	//	Matrix4x4 worldViewProjectionMatrix = MakeIdentityMatrix();
	//	//ビルボードなし
	//	if(billboardTypeEnable[0]){
	//		worldViewProjectionMatrix = particleIterator->transform.GetWorldMatrix() * camera->GetViewProjectionMatrix();
	//	}
	//	//全方位ビルボード
	//	else if(billboardTypeEnable[1]){
	//		billboardMatrix_ = camera->GetWorldMatrix();
	//		billboardMatrix_.m[3][0] = 0.0f;
	//		billboardMatrix_.m[3][1] = 0.0f;
	//		billboardMatrix_.m[3][2] = 0.0f;
	//		
	//		Matrix4x4 worldMatrix = MakeScaleMatrix(particleIterator->transform.scale) * billboardMatrix_ * MakeTranslateMatrix(particleIterator->transform.translate);
	//		worldViewProjectionMatrix = worldMatrix * camera->GetViewProjectionMatrix();
	//	}
	//	//Y軸ビルボード
	//	else if(billboardTypeEnable[2]){
	//		Matrix4x4 cameraMat = MakeAffineMatrix(camera->scale, {0,camera->rotation.y,camera->rotation.z}, camera->translate);

	//		billboardMatrix_ = cameraMat;
	//		billboardMatrix_.m[3][0] = 0.0f;
	//		billboardMatrix_.m[3][1] = 0.0f;
	//		billboardMatrix_.m[3][2] = 0.0f;
	//		
	//		Matrix4x4 worldMatrix = MakeScaleMatrix(particleIterator->transform.scale) * billboardMatrix_ * MakeTranslateMatrix(particleIterator->transform.translate);
	//		worldViewProjectionMatrix = worldMatrix * camera->GetViewProjectionMatrix();
	//	}

	//	//最大数制御
	//	if(numInstance < kNumMaxInstance_){
	//		particleData_[numInstance].WVP = worldViewProjectionMatrix;
	//		particleData_[numInstance].World = worldViewProjectionMatrix;

	//		float alpha = 1.0f - (particleIterator->currentTime / particleIterator->lifeTime);
	//		particleData_[numInstance].color = particleIterator->color;
	//		particleData_[numInstance].color.w = alpha;

	//		++numInstance;
	//	}
	//	++particleIterator;
	//}

	////ルートシグネチャ設定 PSOに設定しいているが別途設定が必要
	//dxCommon->GetCommandList()->SetGraphicsRootSignature(pipeline_->GetRootSignature());
	//dxCommon->GetCommandList()->SetPipelineState(pipeline_->GetGraphicsPipelineState());	//PSO設定
	//dxCommon->GetCommandList()->IASetVertexBuffers(0,1,&vertexBufferView_);		//VBV設定
	//if(isIndexDataEnable_)dxCommon->GetCommandList()->IASetIndexBuffer(&indexBufferView_);		//IBV設定

	////形状設定、PSOに設定しているのとは別
	//dxCommon->GetCommandList()->IASetPrimitiveTopology(commandPrimitiveTopology_);


	////SRV(テクスチャ)のDescriptorTableの先頭を設定 2はRootParamterのインデックスRootParamter[2]
	//dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(0, texture_.srvHandleGPU_);
	////行列のwvpBufferの場所を設定 ※RootParameter[1]に対してCBVの設定
	//dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(1,instancingSrvHandleGPU_);

	////描画
	//isIndexDataEnable_ ? 
	//	dxCommon->GetCommandList()->DrawIndexedInstanced(indexNum_,numInstance,0,0,0) : 
	//	dxCommon->GetCommandList()->DrawInstanced(vertNum_,numInstance,0,0);
}

void ParticleEmitterObject::Add(const Vector3& translate)
{
	//乱数生成器
	std::random_device seedGenerator_;
	std::mt19937 randomEngine(seedGenerator_());
	emitter_.frequencyTime += kDeltaTime_;
	if(emitter_.frequency <= emitter_.frequencyTime){
		particles_.splice(particles_.end(), Emission(emitter_, randomEngine, translate));
		emitter_.frequencyTime -= emitter_.frequency;
	}
}

void ParticleEmitterObject::VertexData()
{
	vertData_[0].position = {-1.0f, 1.0f, 0.0f, 1.0f};		//左上
	vertData_[0].texcoord = {0.0f, 0.0f};
	
	vertData_[1].position = {1.0f, -1.0f, 0.0f, 1.0f};	//右下
	vertData_[1].texcoord = {1.0f, 1.0f};
	
	vertData_[2].position = {-1.0f, -1.0f, 0.0f, 1.0f};	//左下
	vertData_[2].texcoord = {0.0f, 1.0f};
	
	vertData_[3].position = {1.0f, 1.0f, 0.0f, 1.0f};		//右上
	vertData_[3].texcoord = {1.0f, 0.0f};
}

void ParticleEmitterObject::IndexData()
{
	indexData_[0] = 0;	indexData_[1] = 1;	indexData_[2] = 2;
	indexData_[3] = 0;	indexData_[4] = 3;	indexData_[5] = 1;
}

ParticleData ParticleEmitterObject::MakeNewParticle(std::mt19937 &randomEngine, const Vector3& translate)
{
	std::uniform_real_distribution<float> distSizeX(-emitter_.transform.scale.x/2.f ,emitter_.transform.scale.x/2.f);
	std::uniform_real_distribution<float> distSizeY(-emitter_.transform.scale.y/2.f ,emitter_.transform.scale.y/2.f);
	std::uniform_real_distribution<float> distSizeZ(-emitter_.transform.scale.z/2.f ,emitter_.transform.scale.z/2.f);

	std::uniform_real_distribution<float> distVelocity(emitter_.velocity.x,emitter_.velocity.y);
	std::uniform_real_distribution<float> distColor(0.0f,1.0f);
	std::uniform_real_distribution<float> distTime(1.0f,3.0f);
	ParticleData particle;

	particle.transform.scale = {1,1,1};
	particle.transform.rotation = {0,0,0};
	particle.transform.translate = translate + Vector3{distSizeX(randomEngine),distSizeY(randomEngine),distSizeZ(randomEngine)};

	particle.velocity = {distVelocity(randomEngine),distVelocity(randomEngine),distVelocity(randomEngine)};

	particle.color = {distColor(randomEngine),distColor(randomEngine),distColor(randomEngine), 1.0f};
	
	particle.lifeTime = distTime(randomEngine);
	particle.currentTime = 0;

	return particle;
}

std::list<ParticleData> ParticleEmitterObject::Emission(const Emitter &emitter, std::mt19937 &randomEngine, const Vector3& translate)
{
	std::list<ParticleData> particles;
	for(uint32_t i = 0; i < emitter.count; i++){
		particles.push_back(MakeNewParticle(randomEngine, translate));
	}

	return particles;
}
