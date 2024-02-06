#pragma once
#include "Base/GeometryBase.h"

#include "ObjLoader.h"

class ObjModel : public GeometryBase
{
public:
	static ObjModel* Create(std::string filePath, Transform transform = {{0,0,0}, {0,0,0}, {1,1,1}}, BlendSetting::BlendMode mode = BlendSetting::kBlendModeNormal);

public:
	ObjModel(std::string filePath, Transform lTransform, BlendSetting::BlendMode mode){
		filePath_ = filePath;
		translate = lTransform.translate;
		rotation = lTransform.rotation;
		scale = lTransform.scale;

		pipelineDatas.blendMode = mode;
	}

protected:
	void ObjModelLoad();
	void ObjModelVertexData();
	void ObjModelIndexData();

	bool PipelineCreate() override;

private:
	std::string filePath_;

	ObjModelData modelData_;
};

