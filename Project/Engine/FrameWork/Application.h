#pragma once
#include "Camera.h"

#include "Geometry/Sphere.h"
#include "Geometry/ObjModel.h"
#include "Geometry/Line.h"
#include "Geometry/Sprite2D.h"


#include "Object3D/Object3D.h"



#include <vector>
#include <string>

class Application
{
public:
	static Application* Create();

public:
	~Application(){
		//delete obj;
	}

	void Update();
	void Draw();

private:
	void Initialize();

	std::vector<std::string> getImageName(std::string dir_name);

private:
	Camera* camera_ = nullptr;

	std::vector<std::string> files;

	//Object3D* obj;

	//Sphere* obj;
	//ObjModel* obj;
	//Line* obj;
	//Sprite2D* obj;
};

