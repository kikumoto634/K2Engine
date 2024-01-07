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

#include "CollisionManager.h"

#include "Particle/ParticleObject.h"
#include "Particle/ParticleEmitterObject.h"


#include "CollisionPrimitive.h"
#include "CollisionCheck.h"
#include "Input.h"

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
	std::unique_ptr<LevelLoader> levelLoader_;
	std::unique_ptr<Player> player_;
	ObjModel* box_ = nullptr;

	std::unique_ptr<CollisionManager> collisionManager_;

	ParticleObject* particle_ = nullptr;
	Vector3 particlePos_ = {-5,0,0};
	ParticleEmitterObject* emitter_ = nullptr;
	Vector3 emitterPos_ = {5,0,0};

	Sprite2D* sp_ = nullptr;


	SphereCP sphere;
	PlaneCP plane;
	TriangleCP triangle;
};

