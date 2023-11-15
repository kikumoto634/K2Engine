#pragma once
#include <vector>
#include <string>
#include "VertexData.h"

struct ObjMaterialData{
	std::string textureFilePath;
};

struct ObjModelData{
	std::vector<VertexData> vertices;
	ObjMaterialData material;
};
