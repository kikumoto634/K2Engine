#include "ObjLoader.h"
#include <fstream>
#include <sstream>
#include <cassert>

using namespace std;

ObjModelData LoadObjFile(const std::string &fileName)
{
    //必要変数
    ObjModelData modelData; //構築用
    vector<Vector4> positions; //座標
    vector<Vector3> normals;   //法線
    vector<Vector2> texcoords; //UV

    string line;   //一行読み取りライン

    //Open
    string fullPath = "Resources/ObjModels/" + fileName + "/" + fileName + ".obj";
    ifstream file(fullPath);
    assert(file.is_open());

    //読み取り
    while(getline(file, line)){
        string identifier;
        istringstream s(line);
        s >> identifier;    //先頭識別子確認
        ///
        //・ v     : 頂点情報
        //・ vt    : 頂点UV
        //・ vn    : 頂点法線
        //・ g     : 面
        //・ f     : 面情報(インデックス)
        //・ mtlib : テクスチャ
        ///

        if(identifier == "v"){
            Vector4 lpos;
            s >> lpos.x >> lpos.y >> lpos.z;
            lpos.w = 1.0f;
            positions.push_back(lpos);
        }
        else if(identifier == "vt"){
            Vector2 luv;
            s >> luv.x >> luv.y;
            texcoords.push_back(luv);
        }
        else if(identifier == "vn"){
            Vector3 lnor;
            s >> lnor.x >> lnor.y >> lnor.z;
            normals.push_back(lnor);
        }
        else if(identifier == "f")  //三角形の作成
        {
            VertexData triangle[3];

            //面は三角形限定
            for(int32_t faceVertex = 0; faceVertex < 3; ++faceVertex){
                string vertexDefinition;
                s >> vertexDefinition;
                
                //頂点の要素へのIndexは「位置/UV/法線」で格納されているので、分解してIndexを取得
                istringstream v(vertexDefinition);
                uint32_t elementIndices[3];
                for(int32_t element = 0; element < 3; ++element){
                    string index;
                    getline(v, index, '/'); //区切りでインデックスを読んでいく
                    elementIndices[element] = stoi(index);
                }

                //要素へのIndexから、実際の要素の値を取得して、頂点を構築する
                Vector4 lpos = positions[elementIndices[0] - 1];
                Vector2 luv = texcoords[elementIndices[1] - 1];
                Vector3 lnor = normals[elementIndices[2] - 1];

                //反転
                lpos.x *= -1.0f;
                lnor.x *= -1.0f;
                //UV変換
                luv.y = 1.0f - luv.y;

                triangle[faceVertex] = {lpos, luv, lnor};
            }

            //頂点を逆順で登録することで、周り順を逆にする
            modelData.vertices.push_back(triangle[2]);
            modelData.vertices.push_back(triangle[1]);
            modelData.vertices.push_back(triangle[0]);
        }
        else if(identifier == "mtllib"){
            //materialTemplateLibraryファイルの名前を取得
            string materialFileName;
            s >> materialFileName;
            //基本的にobjファイルとそういつ階層にmtlが存在する
            modelData.material = LoadObjMaterialTemplateFile(fileName, materialFileName);
        }
    }
    return modelData;
}

ObjMaterialData LoadObjMaterialTemplateFile(const std::string fileName, const std::string mtlName)
{
    ObjMaterialData materialData;   //構築用
    string line;

    //読み込み
    ifstream file("Resources/ObjModels/" + fileName + "/" + mtlName);
    assert(file.is_open());

    //読み取り
    while(getline(file, line)){
        string identifier;
        istringstream s(line);

        s >> identifier;
        ///
        // map_Kd : テクスチャ名
        ///
        
        if(identifier == "map_Kd"){
            string textureFileName;
            s >> textureFileName;

            materialData.textureFilePath = "ObjModels/" + fileName + "/" + textureFileName;
        }
    }

    return materialData;
}
