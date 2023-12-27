#pragma once
#include "Common/GeometryBase.h"
#include "ObjLoader.h"

class ObjModel : public GeometryBase
{
public:
	static ObjModel* Create(std::string filePath, Transform transform = {{0,0,0}, {0,0,0}, {1,1,1}}, BlendSetting::BlendMode mode = BlendSetting::kBlendModeNormal);

public:
	ObjModel(std::string filePath, Transform lTransform, BlendSetting::BlendMode mode){
		name = "cube";

		filePath_ = filePath;
		translate = lTransform.translate;
		rotation = lTransform.rotation;
		scale = lTransform.scale;

		blendMode = mode;
	}
public:
	virtual void Update(){};

protected:
	void ObjModelLoad();
	void ObjModelVertexData();

private:
	std::string filePath_;

	ObjModelData modelData_;
};

