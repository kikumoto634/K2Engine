#include "LevelLoader.h"

#include <json.hpp>
#include <fstream>
#include <assert.h>

#define PI 3.14159265f


const std::string LevelLoader::kDefaultBaseDirectory_ = "Resources/Levels/";
const std::string LevelLoader::kExtension_ = ".json";

LevelLoader::LevelData* LevelLoader::levelDatas_ = nullptr;
std::vector<ObjModel*> LevelLoader::objects_;

void LevelLoader::Load(const std::string &filename)
{
	const std::string fullpath = kDefaultBaseDirectory_ + filename + kExtension_;

	std::ifstream file{};

	file.open(fullpath);
	if(file.fail()){
		assert(0);
	}

	//JSON文字列から解凍したデータ
	nlohmann::json deserialized;

	//解凍
	file >> deserialized;

	assert(deserialized.is_object());
	assert(deserialized.contains("name"));
	assert(deserialized["name"].is_string());

	//"name"を文字列として取得
	std::string name = deserialized["name"].get<std::string>();
	//正しいレベルデータファイルかチェック
	assert(name.compare("scene") == 0);

	//レベルデータ格納用インスタンス
	LevelData* levelData = new LevelData();

	//"objects"の全オブジェクトを走査
	for(nlohmann::json& object : deserialized["objects"]){
		assert(object.contains("type"));

		//種別を取得
		std::string type = object["type"].get<std::string>();

		//種類ごとの処理
		if(type.compare("MESH") == 0){
			//要素追加
			levelData->objects_.emplace_back(LevelData::ObjectData{});
			//今追加した要素の参照を得る
			LevelData::ObjectData& objectData = levelData->objects_.back();

			if(object.contains("file_name")){
				//ファイル名
				objectData.fileName_ = object["file_name"];

				//トランスフォームのパラメータ読み込み
				nlohmann::json& transform = object["transform"];
				//平行移動
				objectData.transform_.translate.x = -(float)transform["translation"][0];
				objectData.transform_.translate.y = (float)transform["translation"][2];
				objectData.transform_.translate.z = -(float)transform["translation"][1];
				//回転角
				objectData.transform_.rotation.x = (float)transform["rotation"][0]*(180.f/PI) - 90*(PI/180.f);
				objectData.transform_.rotation.y = (float)transform["rotation"][2]*(180.f/PI);
				objectData.transform_.rotation.z = (float)transform["rotation"][1]*(180.f/PI);
				//スケーリング
				objectData.transform_.scale.x = (float)transform["scaling"][0];
				objectData.transform_.scale.y = (float)transform["scaling"][1];
				objectData.transform_.scale.z = (float)transform["scaling"][2];

				//コライダーのパラメータ読み込み
			}
		}


		//再帰関数
		if(object.contains("children")){
			
		}
	}

	//return levelData;
	levelDatas_ = levelData;
}

void LevelLoader::Initialize()
{
	//レベルデータからオブジェクトを生成
	for(auto& objectData : levelDatas_->objects_){
		ObjModel* model = ObjModel::Create(objectData.fileName_, objectData.transform_);
		//配列に登録
		objects_.push_back(model);
	}
}

void LevelLoader::Update()
{
	//レベルデータからオブジェクトを更新
	for(auto& obj : objects_){
		obj->Update();
	}
}

void LevelLoader::Draw(Camera* camera)
{
	//レベルデータからオブジェクトを更新
	for(auto& obj : objects_){
		obj->Draw(camera);
	}
}
