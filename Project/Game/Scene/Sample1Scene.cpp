#include "Sample1Scene.h"
#include "Sample2Scene.h"

void Sample1Scene::Initialize()
{
	camera_ = FollowCamera::GetInstance();

	mapSp_ = Sprite3D::Create("uvChecker.png");
}

void Sample1Scene::Update()
{
	if(KeyboardInput::GetInstance()->Trigger(DIK_SPACE)){
		SceneManager::GetInstance()->SetNext(new Sample2Scene);
	}

	mapSp_->Update();

	camera_->Update({0,0,0});
}

void Sample1Scene::Finalize()
{
	delete mapSp_;
}

void Sample1Scene::GeometryDraw()
{
	mapSp_->Draw(camera_);
}

void Sample1Scene::SpriteDraw()
{
}

void Sample1Scene::ParticleDraw()
{
}
