#include "Sample2Scene.h"

#include "SceneManager.h"
#include "KeyboardInput.h"
#include "Sample1Scene.h"

void Sample2Scene::Initialize()
{
	camera_ = FollowCamera::GetInstance();

	monkey_ = ObjModel::Create("cube");
}

void Sample2Scene::Update()
{
	if(KeyboardInput::GetInstance()->Trigger(DIK_SPACE)){
		SceneManager::GetInstance()->SetNext(new Sample1Scene);
	}

	monkey_->Update();

	camera_->Update({0,0,0});
}

void Sample2Scene::Finalize()
{
	delete monkey_;
}

void Sample2Scene::GeometryDraw()
{
	monkey_->Draw(camera_);
}

void Sample2Scene::SpriteDraw()
{
}

void Sample2Scene::ParticleDraw()
{
}
