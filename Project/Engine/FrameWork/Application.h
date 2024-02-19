#pragma once
#include "Camera.h"
#include "LightingGroup.h"

#include "Geometry/Sphere.h"
#include "Geometry/ObjModel.h"
#include "Geometry/Sprite2D.h"

#include "SpriteLoader.h"

//シーンオブジェクト
#include "LevelLoader.h"
#include "../../Game/Object/Character/Player.h"
#include "FollowCamera.h"

#include "Particle/ParticleObject.h"
#include "Particle/ParticleEmitterObject.h"

#include "CollisionsManager.h"

#include "Particle/Base/GPUParticleBase.h"
#include "../../TestObj.h"

class Application
{
public:
	static Application* Create();

public:
	~Application(){
		delete camera_;
		delete light_;

		delete move;

		/*for(ObjModel* v : model_){
			delete v;
		}*/

		delete testModel_;
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
	const int NumX = 1;
	const int NumY = 1;
	const int NumZ = 1;
	std::vector<ObjModel*> model_;
	TestObj* testModel_;

	ObjModel* move= nullptr;
	float time_ = 0.0f;
};

