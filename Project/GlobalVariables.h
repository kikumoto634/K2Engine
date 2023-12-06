#pragma once
#include <variant>
#include <map>
#include <string>

#include "Vector3.h"

/// <summary>
/// グローバル変数
/// </summary>
class GlobalVariables
{
public:
	static GlobalVariables* GetInstance();

public:
	/// <summary>
	/// Group作成
	/// </summary>
	/// <param name="groupName">グループ名</param>
	void CreateGroup(const std::string& groupName);

	//更新
	void Update();

	//値のセット(int)
	void SetValue(const std::string& groupName, const std::string& key, int32_t value);
	//値のセット(float)
	void SetValue(const std::string& groupName, const std::string& key, float value);
	//値のセット(Vector3)
	void SetValue(const std::string& groupName, const std::string& key, const Vector3& value);

private:
	GlobalVariables() = default;
	~GlobalVariables() = default;
	GlobalVariables(const GlobalVariables& other) = default;
	GlobalVariables& operator=(const GlobalVariables& other) = default;

private:
	static GlobalVariables* instance_;

private:
	//項目
	struct Item{
		//項目の値
		std::variant<int32_t, float, Vector3> value;
	};

	//グループ
	struct Group{
		std::map<std::string, Item> items;
	};

	//全データ
	std::map<std::string, Group> data_;
};

