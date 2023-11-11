#include "ImGuiManager.h"

#include "../../External/imgui/imgui.h"
#include "../../External/imgui/imgui_impl_dx12.h"
#include "../../External/imgui/imgui_impl_win32.h"

void ImGuiManager::Initialize(HWND hwnd, DirectXCommon *dxCommon)
{
	//ImGui初期化
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX12_Init(
		dxCommon->GetDevice(),
		dxCommon->GetSwapChainDesc().BufferCount,
		dxCommon->GetRTVDesc().Format,
		dxCommon->GetSRVDescriptorHeap(),
		dxCommon->GetSRVDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
		dxCommon->GetSRVDescriptorHeap()->GetGPUDescriptorHandleForHeapStart()
	);
}

void ImGuiManager::NewFrame()
{
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImGuiManager::CreateCommand()
{
	//ImGuiの内部コマンドを生成する
	ImGui::Render();
}

void ImGuiManager::CommandsExcute(ID3D12GraphicsCommandList *commandList)
{
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
}


ImGuiManager* ImGuiManager::Create(){

	ImGuiManager* instance = new ImGuiManager();
	return instance;
}

ImGuiManager::~ImGuiManager()
{
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiManager::ShowDemo(){
	ImGui::ShowDemoWindow();
}
