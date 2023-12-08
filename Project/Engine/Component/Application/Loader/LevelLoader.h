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
			std::string fileName;
			//トランスフォーム情報
			Transform transform;
		};

		// オブジェクト配列
		std::vector<ObjectData> objects;
	};

private:
	static const std::string kDefaultBaseDirectory;
	static const std::string kExtension;

public:
	static void Load(const std::string& filename);

	static void Initialize();
	static void Update();
	static void Draw(const Matrix4x4& viewProjectionMatrix);

private:
	static LevelData* levelDatas;
	static std::vector<ObjModel*> objects;
};

