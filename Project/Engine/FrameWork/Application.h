#pragma once
#include "Camera.h"
#include "LightingGroup.h"

#include "Geometry/Sphere.h"
#include "Geometry/ObjModel.h"
#include "Geometry/Sprite2D.h"

#include "SpriteLoader.h"

//シーンオブジェクト
//#include "LevelLoader.h"
//#include "../../Game/Object/Character/Player.h"
#include "FollowCamera.h"
//
//#include "Particle/ParticleObject.h"
//#include "Particle/ParticleEmitterObject.h"
//
//#include "CollisionsManager.h"
//
//#include "Particle/Base/GPUParticleBase.h"

#include "SceneManager.h"
#include "BaseScene.h"

class Application
{
public:
	static Application* Create();

public:
	~Application(){
		delete camera_;
		delete light_;
	}

	void Update();
	void GeometryDraw();
	void SpriteDraw();
	void ParticleDraw();

private:
	void Initialize();

	void CollisionCheck();

private:
	FollowCamera* camera_ = nullptr;
	LightingGroup* light_ = nullptr;

	//シーンオブジェクト
	
	//std::unique_ptr<LevelLoader> levelLoader_;
	//ObjModel* box_ = nullptr;
	/*Sprite2D* sp_ = nullptr;
	ParticleObject* particle_ = nullptr;
	Vector3 particlePos_ = {-5,0,0};
	
	Vector3 emitterPos_ = {5,0,0};
	ParticleEmitterObject* emitter_ = nullptr;*/
	
	//std::unique_ptr<Player> player_;
	//CollisionsManager* collisionsManager = nullptr;*/

	//GPUParticleBase* gpu_ = nullptr;

	SceneManager* sceneManager_ = nullptr;
	BaseScene* scene_ = nullptr;
};

