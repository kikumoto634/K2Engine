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

class Application
{
public:
	static Application* Create();

public:
	~Application(){
		delete camera_;
		delete light_;

		for(ObjModel* v : obj_){
			delete v;
		}
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
	const int CreateNum = 10;
	std::vector<ObjModel*> obj_;
};

