#pragma once
#include <vector>
#include <string>
#include "VertexData.h"

struct ObjMaterialData{
	std::string textureFilePath;
};

struct ObjModelData{
	std::vector<VertexData> vertices;
	std::vector<uint32_t> indices;
	ObjMaterialData material;
};
