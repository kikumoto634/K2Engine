#include "Application.h"

#include "WindowsApp.h"
#include <imgui.h>

Application *Application::Create()
{
	Application* instance = new Application();
	instance->Initialize();
	return instance;
}

void Application::Initialize()
{
	camera_ = Camera::Create();

	files = getImageName("./Resources/");

	//obj = Object3D::Create();

	//obj = Sphere::Create();
	//obj = ObjModel::Create("cube");
	//obj = Line::Create();
	//obj = Sprite2D::Create();
}

std::vector<std::string> Application::getImageName(std::string dir_name)
{
	HANDLE hFind;
	WIN32_FIND_DATA win32d;
	std::vector<std::wstring> filaNames;

	//.png, .jpgの拡張子のみ読み込む
	std::string extension[2] = {"png", "jpg"};

	for(int i = 0; i < 2; i++){
		std::string searchName = dir_name + "*." + extension[i];
		std::wstring str = WindowsApp::ConvertString(searchName);
		hFind = FindFirstFile(str.c_str(), &win32d);

		if(hFind == INVALID_HANDLE_VALUE){
			continue;
		}
		do{
			if(win32d.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
			}
			else{
				filaNames.push_back(win32d.cFileName);
			}
		}while(FindNextFile(hFind, &win32d));

		FindClose(hFind);
	}

	std::vector<std::string> vec;
	vec.resize(filaNames.size());
	for(int i = 0; i < filaNames.size(); i++){
		vec[i] = WindowsApp::ConvertString(filaNames[i]);
	}
	return vec;
}


void Application::Update()
{
	camera_->Update();

	std::string atr = "Test";

	ImGui::Text("%s", files[0].c_str());
	ImGui::Text("%s", files[1].c_str());
	ImGui::Text("%s", files[2].c_str());
	ImGui::Text("%s", files[3].c_str());

	//obj->Update();
}

void Application::Draw()
{
	//obj->Draw(camera_->GetViewProjectionMatrix());
}