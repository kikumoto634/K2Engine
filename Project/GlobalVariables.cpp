#include "GlobalVariables.h"
#include "imgui.h"

GlobalVariables* GlobalVariables::instance_ = nullptr;

GlobalVariables *GlobalVariables::GetInstance()
{
	if(!instance_){
		instance_ = new GlobalVariables;
	}
	return instance_;
}

void GlobalVariables::CreateGroup(const std::string &groupName)
{
	//指定名のオブジェクトがなければ追加する
	data_[groupName];
}

void GlobalVariables::Update()
{
	if(!ImGui::Begin("Global Variables", nullptr, ImGuiWindowFlags_MenuBar))
		return;
	if(!ImGui::BeginMenuBar())
		return;

	//各Groupについて
	for(std::map<std::string, Group>::iterator itGroup = data_.begin();
		itGroup != data_.end(); ++itGroup){
		//グループ名を取得
		const std::string groupName = itGroup->first;
		//グループの参照を取得
		Group& group = itGroup->second;

		if(!ImGui::BeginMenu(groupName.c_str()))
			continue;

		//各項目について
		for(std::map<std::string, Item>::iterator itItem = group.items.begin();
			itItem != group.items.end(); ++itItem){
			//項目名を取得
			const std::string& itemName = itItem->first;
			//項目の参照を取得
			Item& item = itItem->second;


			//int32_t型の値を保持していれば
			if(std::holds_alternative<int32_t>(item.value)){
				int32_t* ptr = std::get_if<int32_t>(&item.value);
				ImGui::SliderInt(itemName.c_str(), ptr, 0, 100);
			}
			//float型の値を保持していれば
			else if(std::holds_alternative<float>(item.value)){
				float* ptr = std::get_if<float>(&item.value);
				ImGui::SliderFloat(itemName.c_str(), ptr, 0.0f, 100.0f);
			}
			//Vector3型の値を保持していれば
			else if(std::holds_alternative<Vector3>(item.value)){
				Vector3* ptr = std::get_if<Vector3>(&item.value);
				ImGui::SliderFloat3(itemName.c_str(), reinterpret_cast<float*>(ptr), -10.0f, 10.0f);
			}
		}

		ImGui::EndMenu();
	}


	ImGui::EndMenuBar();
	ImGui::End();
}


void GlobalVariables::SetValue(const std::string &groupName, const std::string &key, int32_t value)
{
	//グループの参照を取得
	Group& group = data_[groupName];
	//新しい項目のdataを設定
	Item newItem;
	newItem.value = value;
	//設定した項目をstd::mapに追加
	group.items[key] = newItem;
}

void GlobalVariables::SetValue(const std::string &groupName, const std::string &key, float value)
{
	//グループの参照を取得
	Group& group = data_[groupName];
	//新しい項目のdataを設定
	Item newItem;
	newItem.value = value;
	//設定した項目をstd::mapに追加
	group.items[key] = newItem;
}

void GlobalVariables::SetValue(const std::string &groupName, const std::string &key, const Vector3 &value)
{
	//グループの参照を取得
	Group& group = data_[groupName];
	//新しい項目のdataを設定
	Item newItem;
	newItem.value = value;
	//設定した項目をstd::mapに追加
	group.items[key] = newItem;
}
