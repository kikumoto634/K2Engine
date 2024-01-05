#pragma once
#include <string>
#include <vector>

#include "../Transform/Transform.h"
#include "Geometry/ObjModel.h"

class LevelLoader
{
private:
	// レベルデータ
	struct LevelData {

		struct ObjectData {
			// ファイル名
			std::string fileName_;
			//トランスフォーム情報
			Transform transform_;
		};

		// オブジェクト配列
		std::vector<ObjectData> objects_;
	};

private:
	static const std::string kDefaultBaseDirectory_;
	static const std::string kExtension_;

public:
	static void Load(const std::string& filename);

	static void Initialize();
	static void Update();
	static void Draw(Camera* camera);

private:
	static LevelData* levelDatas_;
	static std::vector<ObjModel*> objects_;
};

