#include "EmitterFrameSquare.h"

EmitterFrameSquare *EmitterFrameSquare::Create()
{
	EmitterFrameSquare* instance = new EmitterFrameSquare();
	instance->Initialize();
	instance->VertexData();
	instance->IndexData();
	return instance;
}

void EmitterFrameSquare::Update(EmitterData emitter)
{
	translate = emitter.transform.translate;
	rotation = emitter.transform.rotation;
	scale = emitter.transform.scale;
}

void EmitterFrameSquare::VertexData()
{
	vertData_[0] = {-0.5f,-0.5f,-0.5f, 1.0f};
	vertData_[1] = { 0.5f,-0.5f,-0.5f, 1.0f};
	vertData_[2] = { 0.5f, 0.5f,-0.5f, 1.0f};
	vertData_[3] = {-0.5f, 0.5f,-0.5f, 1.0f};

	vertData_[4] = {-0.5f,-0.5f, 0.5f, 1.0f};
	vertData_[5] = { 0.5f,-0.5f, 0.5f, 1.0f};
	vertData_[6] = { 0.5f, 0.5f, 0.5f, 1.0f};
	vertData_[7] = {-0.5f, 0.5f, 0.5f, 1.0f};
}

void EmitterFrameSquare::IndexData()
{
	indexData_[0] = 3;	indexData_[1] = 2;	indexData_[2] = 1;
	indexData_[3] = 0;	indexData_[4] = 3;	indexData_[5] = 1;
	
	indexData_[6] = 5;	indexData_[7] = 6;	indexData_[8] = 7;
	indexData_[9] = 5;	indexData_[10] = 7;	indexData_[11] = 4;
	
	indexData_[12] = 7;	indexData_[13] = 3;	indexData_[14] = 0;
	indexData_[15] = 0;	indexData_[16] = 4;	indexData_[17] = 7;
	
	indexData_[18] = 6;	indexData_[19] = 5;	indexData_[20] = 1;
	indexData_[21] = 1;	indexData_[22] = 2;	indexData_[23] = 6;
	
	indexData_[24] = 6;	indexData_[25] = 2;	indexData_[26] = 3;
	indexData_[27] = 3;	indexData_[28] = 7;	indexData_[29] = 6;

	indexData_[30] = 0;	indexData_[31] = 1;	indexData_[32] = 5;
	indexData_[33] = 5;	indexData_[34] = 4;	indexData_[35] = 0;
}
