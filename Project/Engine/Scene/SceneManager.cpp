#include "SceneManager.h"
#include "BaseScene.h"

SceneManager* SceneManager::instance_ = nullptr;

SceneManager *SceneManager::GetInstance()
{
    if(!instance_){
        instance_ = new SceneManager();
    }

    return instance_;
}

void SceneManager::Finalize()
{
    if(instance_){
        delete instance_;
        instance_ = nullptr;
    }
}

void SceneManager::Initialize(BaseScene* scene)
{
    next_ = scene;
}

void SceneManager::Update()
{
    if(next_){
        if(current_){
            current_->Finalize();
            delete current_;
            current_ = nullptr;
        }

        current_ = next_;
        current_->Initialize();

        next_= nullptr;
    }
    current_->Update();
}

void SceneManager::Draw()
{
    current_->SpriteDraw();
    current_->GeometryDraw();
    current_->ParticleDraw();
}
