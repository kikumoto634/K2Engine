#pragma once
#include "Common/GeometryBase.h"
#include "ObjLoader.h"

class ObjModel : public GeometryBase
{
public:
	static ObjModel* Create(std::string filePath, Transform transform = {{0,0,0}, {0,0,0}, {1,1,1}});

public:
	ObjModel(std::string filePath, Transform lTransform){
		filePath_ = filePath;
		transform = lTransform;
	}
	void Update();

private:
	void ObjModelLoad();
	void ObjModelVertexData();

private:
	std::string filePath_;

	ObjModelData modelData_;
};

