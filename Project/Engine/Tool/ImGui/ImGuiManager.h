#pragma once
#include "DirectXCommon.h"

class ImGuiManager
{
public:
	//初期化
	static void Initialize(HWND hwnd, DirectXCommon* dxCommon);
	
	//更新開始
	static void NewFrame();
	
	//更新終了、コマンド生成
	static void CreateCommand();

	//コマンド積む
	static void CommandsExcute(ID3D12GraphicsCommandList* commandList);

	//new生成
	static ImGuiManager* Create();

public:
	~ImGuiManager()	;

public:
	void ShowDemo();
};

