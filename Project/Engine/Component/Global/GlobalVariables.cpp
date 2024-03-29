#include "GlobalVariables.h"
#include <imgui.h>

#include <json.hpp>
#include <fstream>
#include <Windows.h>

GlobalVariables* GlobalVariables::instance_ = nullptr;

using json = nlohmann::json;

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

void GlobalVariables::SaveFile(const std::string &groupName)
{
	//グループを検索
	std::map<std::string, Group>::iterator itGroup = data_.find(groupName);

	//未登録チェック
	assert(itGroup != data_.end());

	json root;
	root = json::object();
	//jsonオブジェクト登録
	root[groupName] = json::object();

	//各項目について
	for(std::map<std::string, Item>::iterator itItem = itGroup->second.items.begin();
		itItem != itGroup->second.items.end(); ++itItem){
		//項目名を取得
		const std::string& itemName = itItem->first;
		//項目の参照を取得
		Item& item = itItem->second;


		//int32_t型の値を保持していれば
		if(std::holds_alternative<int32_t>(item.value)){
			//int32_t型の値を登録
			root[groupName][itemName] = std::get<int32_t>(item.value);
		}
		//float型の値を保持していれば
		else if(std::holds_alternative<float>(item.value)){
			//float型の値を登録
			root[groupName][itemName] = std::get<float>(item.value);
		}
		//Vector2型の値を保持していれば
		else if(std::holds_alternative<Vector2>(item.value)){
			//Vector3型の値を登録
			Vector2 value = std::get<Vector2>(item.value);
			root[groupName][itemName] = json::array({value.x, value.y});
		}
		//Vector3型の値を保持していれば
		else if(std::holds_alternative<Vector3>(item.value)){
			//Vector3型の値を登録
			Vector3 value = std::get<Vector3>(item.value);
			root[groupName][itemName] = json::array({value.x, value.y, value.z});
		}
		//Vector4型の値を保持していれば
		else if(std::holds_alternative<Vector4>(item.value)){
			//Vector4型の値を登録
			Vector4 value = std::get<Vector4>(item.value);
			root[groupName][itemName] = json::array({value.x, value.y, value.z, value.w});
		}
	}

	//ディレクトリが無ければ作成する
	std::filesystem::path dir(kDirectorPath);
	if(!std::filesystem::exists(dir)){
		std::filesystem::create_directory(dir);
	}

	//書き込むJSONファイルのフルパス合成
	std::string filePath = kDirectorPath + groupName + ".json";
	//書き込み用ファイルストリーム
	std::ofstream ofs;
	//ファイルを書き込み用に開く
	ofs.open(filePath);

	//ファイルオープンエラー
	if(ofs.fail()){
		std::string message = "GlobalVariables : Failed open data file for write";
		MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
		assert(0);
		return;
	}

	//ファイルにjson文字列を書き込む(インデント幅4)
	ofs << std::setw(4) << root << std::endl;
	//ファイルクローズ
	ofs.close();
}


void GlobalVariables::LoadFiles()
{
	std::filesystem::path dir(kDirectorPath);
	//ディレクトリがなければスキップする
	if(!std::filesystem::exists(dir)){
		return;
	}

	std::filesystem::directory_iterator dir_it(dir);
	for(const std::filesystem::directory_entry& entry : dir_it){
		//ファイルパス取得
		const std::filesystem::path& filePath = entry.path();
		//ファイル拡張子を取得
		std::string extension = filePath.extension().string();
		//.jsonファイル以外はスキップ
		if(extension.compare(".json") != 0){
			continue;
		}

		//ファイル読み込み
		LoadFile(filePath.stem().string());
	}
}

void GlobalVariables::LoadFile(const std::string &groupName)
{
	//読み込むJSONファイルのフルパス合成
	std::string filePath = kDirectorPath + groupName+ ".json";
	//読み込み用ファイルストリーム
	std::ifstream ifs;
	//ファイルを読み込み用に開く
	ifs.open(filePath);

	//ファイルオープンエラー
	if(ifs.fail()){
		std::string message = "GlobalVariables : Failed open data file for Load";
		MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
		assert(0);
		return;
	}

	json root;
	//json文字列からjsonのデータ構造に展開
	ifs >> root;
	//ファイルを閉じる
	ifs.close();

	//グループ検索
	json::iterator itGroup = root.find(groupName);
	//未登録チェック
	assert(itGroup != root.end());

	//各アイテム
	for(json::iterator itItem = itGroup->begin();
		itItem != itGroup->end(); ++itItem){
		//アイテム名を取得
		const std::string& itemName = itItem.key();

		//int32_t型の値を保持していれば
		if(itItem->is_number_integer()){
			//int32_t型の値を登録
			int32_t value = itItem->get<int32_t>();
			SetValue(groupName, itemName, value);
		}
		//float型の値を保持していれば
		else if(itItem->is_number_float()){
			//float型の値を登録
			float value = itItem->get<float>();
			SetValue(groupName, itemName, value);
		}
		//Vector2型の値を保持していれば
		else if(itItem->is_array() && itItem->size() == 2){
			//Vector2型の値を登録
			Vector2 value = {itItem->at(0), itItem->at(1)};
			SetValue(groupName, itemName, value);
		}
		//Vector3型の値を保持していれば
		else if(itItem->is_array() && itItem->size() == 3){
			//Vector3型の値を登録
			Vector3 value = {itItem->at(0), itItem->at(1), itItem->at(2)};
			SetValue(groupName, itemName, value);
		}
		//Vector4型の値を保持していれば
		else if(itItem->is_array() && itItem->size() == 4){
			//Vector4型の値を登録
			Vector4 value = {itItem->at(0), itItem->at(1), itItem->at(2), itItem->at(3)};
			SetValue(groupName, itemName, value);
		}
	}
}


void GlobalVariables::Update()
{
	ImGui::SetNextWindowPos({0,0});
	ImGui::SetNextWindowSize({350,500});
	ImGui::Begin("Global Variables", nullptr, ImGuiWindowFlags_MenuBar);
	ImGui::BeginMenuBar();
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
				ImGui::DragInt(itemName.c_str(), ptr);
			}
			//float型の値を保持していれば
			else if(std::holds_alternative<float>(item.value)){
				float* ptr = std::get_if<float>(&item.value);
				ImGui::DragFloat(itemName.c_str(), ptr, 0.05f);
			}
			//Vector2型の値を保持していれば
			else if(std::holds_alternative<Vector2>(item.value)){
				Vector2* ptr = std::get_if<Vector2>(&item.value);
				ImGui::DragFloat2(itemName.c_str(), reinterpret_cast<float*>(ptr), 0.1f);
			}
			//Vector3型の値を保持していれば
			else if(std::holds_alternative<Vector3>(item.value)){
				Vector3* ptr = std::get_if<Vector3>(&item.value);
				ImGui::DragFloat3(itemName.c_str(), reinterpret_cast<float*>(ptr), 0.1f);
			}
			//Vector4型の値を保持していれば
			else if(std::holds_alternative<Vector4>(item.value)){
				Vector4* ptr = std::get_if<Vector4>(&item.value);
				ImGui::DragFloat4(itemName.c_str(), reinterpret_cast<float*>(ptr), 0.1f);
			}
		}

		//改行
		ImGui::Text("\n");
		if(ImGui::Button("Save")){
			SaveFile(groupName);
			std::string message = std::format("{}.json saved.", groupName);
			MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
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

void GlobalVariables::SetValue(const std::string &groupName, const std::string &key, const Vector2 &value)
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

void GlobalVariables::SetValue(const std::string &groupName, const std::string &key, const Vector4 &value)
{
	//グループの参照を取得
	Group& group = data_[groupName];
	//新しい項目のdataを設定
	Item newItem;
	newItem.value = value;
	//設定した項目をstd::mapに追加
	group.items[key] = newItem;
}

void GlobalVariables::AddItem(const std::string &groupName, const std::string &key, int32_t value)
{
	//指定のグループのアイテムを取得
	auto itItem = data_[groupName].items.find(key);
	
	//dataないに登録されていない
	if(itItem == data_[groupName].items.end()){
		SetValue(groupName, key, value);
		return;
	}
	return;
}

void GlobalVariables::AddItem(const std::string &groupName, const std::string &key, float value)
{
	//指定のグループのアイテムを取得
	auto itItem = data_[groupName].items.find(key);
	
	//dataないに登録されていない
	if(itItem == data_[groupName].items.end()){
		SetValue(groupName, key, value);
		return;
	}
	return;
}

void GlobalVariables::AddItem(const std::string &groupName, const std::string &key, Vector2 value)
{
	//指定のグループのアイテムを取得
	auto itItem = data_[groupName].items.find(key);
	
	//dataないに登録されていない
	if(itItem == data_[groupName].items.end()){
		SetValue(groupName, key, value);
		return;
	}
	return;
}

void GlobalVariables::AddItem(const std::string &groupName, const std::string &key, Vector3 value)
{
	//指定のグループのアイテムを取得
	auto itItem = data_[groupName].items.find(key);
	
	//dataないに登録されていない
	if(itItem == data_[groupName].items.end()){
		SetValue(groupName, key, value);
		return;
	}
	return;
}

void GlobalVariables::AddItem(const std::string &groupName, const std::string &key, Vector4 value)
{
	//指定のグループのアイテムを取得
	auto itItem = data_[groupName].items.find(key);
	
	//dataないに登録されていない
	if(itItem == data_[groupName].items.end()){
		SetValue(groupName, key, value);
		return;
	}
	return;
}


int32_t GlobalVariables::GetIntValue(const std::string &groupName, const std::string &key) const
{
	//指定のグループの有無
	//指定のグループの有無
	assert(data_.contains(groupName));

	//グループ参照取得
	const Group& group = data_.at(groupName);

	//指定のキーの有無
	assert(group.items.contains(key));

	//項目の参照を取得
	return get<int32_t>(group.items.at(key).value);
}

float GlobalVariables::GetFloatValue(const std::string &groupName, const std::string &key) const
{
	//指定のグループの有無
	assert(data_.contains(groupName));

	//グループ参照取得
	const Group& group = data_.at(groupName);

	//指定のキーの有無
	assert(group.items.contains(key));

	//項目の参照を取得
	return get<float>(group.items.at(key).value);
}

Vector2 GlobalVariables::GetVector2Value(const std::string &groupName, const std::string &key) const
{
	//指定のグループの有無
	assert(data_.contains(groupName));

	//グループ参照取得
	const Group& group = data_.at(groupName);

	//指定のキーの有無
	assert(group.items.contains(key));

	//項目の参照を取得
	return get<Vector2>(group.items.at(key).value);
}

Vector3 GlobalVariables::GetVector3Value(const std::string &groupName, const std::string &key) const
{
	//指定のグループの有無
	assert(data_.contains(groupName));

	//グループ参照取得
	const Group& group = data_.at(groupName);

	//指定のキーの有無
	assert(group.items.contains(key));

	//項目の参照を取得
	return get<Vector3>(group.items.at(key).value);
}

Vector4 GlobalVariables::GetVector4Value(const std::string &groupName, const std::string &key) const
{
	//指定のグループの有無
	assert(data_.contains(groupName));

	//グループ参照取得
	const Group& group = data_.at(groupName);

	//指定のキーの有無
	assert(group.items.contains(key));

	//項目の参照を取得
	return get<Vector4>(group.items.at(key).value);
}
