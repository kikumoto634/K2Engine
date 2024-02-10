#pragma once
#include "Test.h"

#include "ObjLoader.h"

class TestObj : public Test
{
public:
	static TestObj* Create(std::string filePath, Transform transform = {{0,0,0}, {0,0,0}, {1,1,1}}, BlendSetting::BlendMode mode = BlendSetting::kBlendModeNormal);

public:
	TestObj(std::string filePath, Transform lTransform, BlendSetting::BlendMode mode){
		filePath_ = filePath;
		translate = lTransform.translate;
		rotation = lTransform.rotation;
		scale = lTransform.scale;
	}

protected:
	void ObjModelLoad();
	void ObjModelVertexData();

	bool PipelineCreate() override;

private:
	std::string filePath_;

	ObjModelData modelData_;
};

