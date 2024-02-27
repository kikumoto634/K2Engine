#include "Sample1Scene.h"

#include "SceneManager.h"
#include "KeyboardInput.h"
#include "Sample2Scene.h"

void Sample1Scene::Initialize()
{
	camera_ = FollowCamera::GetInstance();

	monkey_ = ObjModel::Create("monkey");
}

void Sample1Scene::Update()
{
	if(KeyboardInput::GetInstance()->Trigger(DIK_SPACE)){
		SceneManager::GetInstance()->SetNext(new Sample2Scene);
	}

	monkey_->Update();

	camera_->Update({0,0,0});
}

void Sample1Scene::Finalize()
{
	delete monkey_;
}

void Sample1Scene::GeometryDraw()
{
	monkey_->Draw(camera_);
}

void Sample1Scene::SpriteDraw()
{
}

void Sample1Scene::ParticleDraw()
{
}
