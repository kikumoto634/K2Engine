#pragma once
#include "Camera.h"

#include "Geometry/Sphere.h"
#include "Geometry/ObjModel.h"
#include "Geometry/Line.h"
#include "Geometry/Sprite2D.h"


#include "Object3D/Object3D.h"

#include "SpriteLoader.h"

#include <vector>
#include <string>

class Application
{
public:
	static Application* Create();

public:
	~Application(){
		delete obj;
	}

	void Update();
	void Draw();

private:
	void Initialize();

private:
	Camera* camera_ = nullptr;

	//Object3D* obj;

	Sphere* obj;
	//ObjModel* obj;
	//Line* obj;
	//Sprite2D* obj;
};

