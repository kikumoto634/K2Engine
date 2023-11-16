#include "FrameWork.h"

//Windowsアプロでのエントリーポイント
int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int){
	
	FrameWork* frameWork_ = FrameWork::Create();
	frameWork_->Run();
	delete frameWork_;

	return 0;
}