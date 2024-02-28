#include "Sample1Scene.h"
#include "Sample2Scene.h"

void Sample1Scene::Initialize()
{
	camera_ = FollowCamera::GetInstance();

	for(int y = 0; y < Height; y++){
		for(int x = 0; x < Width; x++){
			Sprite3D* temp = Sprite3D::Create("uvChecker.png");
		}
	}
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
