#pragma once
#include "Camera.h"
#include "Input.h"
#include "LightingGroup.h"

#include "Geometry/Sphere.h"
#include "Geometry/ObjModel.h"
#include "Geometry/Line.h"
#include "Geometry/Sprite2D.h"


#include "Object3D/Object3D.h"

#include "SpriteLoader.h"

class Application
{
public:
	static Application* Create();

public:
	~Application(){
		delete obj2;
		delete obj;
	}

	void Update();
	void Draw();

private:
	void Initialize();

private:
	Camera* camera_ = nullptr;
	Input* input_ = nullptr;
	LightingGroup* light_ = nullptr;

	//Object3D* obj;

	Sphere* obj;
	ObjModel* obj2;
	//Line* obj;
	//Sprite2D* obj;
};

