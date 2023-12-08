#include "LevelLoader.h"

#include <json.hpp>
#include <fstream>
#include <assert.h>

#define PI 3.14159265f


const std::string LevelLoader::kDefaultBaseDirectory = "Resources/Levels/";
const std::string LevelLoader::kExtension = ".json";

LevelLoader::LevelData* LevelLoader::levelDatas = nullptr;
std::vector<ObjModel*> LevelLoader::objects;

void LevelLoader::Load(const std::string &filename)
{
	const std::string fullpath = kDefaultBaseDirectory + filename + kExtension;

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
			levelData->objects.emplace_back(LevelData::ObjectData{});
			//今追加した要素の参照を得る
			LevelData::ObjectData& objectData = levelData->objects.back();

			if(object.contains("file_name")){
				//ファイル名
				objectData.fileName = object["file_name"];

				//トランスフォームのパラメータ読み込み
				nlohmann::json& transform = object["transform"];
				//平行移動
				objectData.transform.translate.x = -(float)transform["translation"][0];
				objectData.transform.translate.y = (float)transform["translation"][2];
				objectData.transform.translate.z = -(float)transform["translation"][1];
				//回転角
				objectData.transform.rotation.x = (float)transform["rotation"][0]*(180.f/PI) - 90*(PI/180.f);
				objectData.transform.rotation.y = (float)transform["rotation"][2]*(180.f/PI);
				objectData.transform.rotation.z = (float)transform["rotation"][1]*(180.f/PI);
				//スケーリング
				objectData.transform.scale.x = (float)transform["scaling"][0];
				objectData.transform.scale.y = (float)transform["scaling"][1];
				objectData.transform.scale.z = (float)transform["scaling"][2];

				//コライダーのパラメータ読み込み
			}
		}


		//再帰関数
		if(object.contains("children")){
			
		}
	}

	//return levelData;
	levelDatas = levelData;
}

void LevelLoader::Initialize()
{
	//レベルデータからオブジェクトを生成
	for(auto& objectData : levelDatas->objects){
		ObjModel* model = ObjModel::Create(objectData.fileName, objectData.transform);
		//配列に登録
		objects.push_back(model);
	}
}

void LevelLoader::Update()
{
	//レベルデータからオブジェクトを更新
	for(auto& obj : objects){
		obj->Update();
	}
}

void LevelLoader::Draw(const Matrix4x4 &viewProjectionMatrix)
{
	//レベルデータからオブジェクトを更新
	for(auto& obj : objects){
		obj->Draw(viewProjectionMatrix);
	}
}
