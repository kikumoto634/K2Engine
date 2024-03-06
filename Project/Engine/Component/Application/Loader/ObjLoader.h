#pragma once
#include <string>

#include "GeometryDatas/ObjModelData.h"

//Objファイル読み込み
ObjModelData LoadObjFile(const std::string& fileName);

//Objファイルマテリアル読み込み
ObjMaterialData LoadObjMaterialTemplateFile(const std::string fileName, const std::string mtlName);

