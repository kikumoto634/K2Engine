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

	/// <summary>
	/// ファイル書き出し
	/// </summary>
	/// <param name="groupName">グループ</param>
	void SaveFile(const std::string& groupName);

	/// <summary>
	/// ディレクトリの全ファイル読み込み
	/// </summary>
	void LoadFiles();

	/// <summary>
	/// ファイル読み込み
	/// </summary>
	/// <param name="groupName">グループ</param>
	void LoadFile(const std::string& groupName);


	//更新
	void Update();

	//値のセット(int)
	void SetValue(const std::string& groupName, const std::string& key, int32_t value);
	//値のセット(float)
	void SetValue(const std::string& groupName, const std::string& key, float value);
	//値のセット(Vector3)
	void SetValue(const std::string& groupName, const std::string& key, const Vector3& value);

	//項目の追加(int32_t)
	void AddItem(const std::string& groupName, const std::string& key, int32_t value);
	//項目の追加(float)
	void AddItem(const std::string& groupName, const std::string& key, float value);
	//項目の追加(Vector3)
	void AddItem(const std::string& groupName, const std::string& key, Vector3 value);


	//Getter
	int32_t GetIntValue(const std::string& groupName, const std::string& key) const;
	float GetFloatValue(const std::string& groupName, const std::string& key) const;
	Vector3 GetVector3Value(const std::string& groupName, const std::string& key) const;

private:
	GlobalVariables() = default;
	~GlobalVariables() = default;
	GlobalVariables(const GlobalVariables& other) = default;
	GlobalVariables& operator=(const GlobalVariables& other) = default;

private:
	static GlobalVariables* instance_;

private:
	//Global変数の保存先ファイルパス
	const std::string kDirectorPath = "Resources/GlovalVariables/";

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

