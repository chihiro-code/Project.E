#pragma once
#include "ColliderComponent.h"


//***********************************************
// グリットマップの大きさを定義（テキストデータの縦横の数と合わせる）
//***********************************************
#define GRIDMAP_WIDTH  50
#define GRIDMAP_HEIGHT 50


//***********************************************
// Mapの種類のIDリストを定義する
//***********************************************
enum MAP_LIST {
	GRIDMAP_01,

	MAX_MAP_LIST
};


//***********************************************
// Map配置のテキストデータのファイルパス
//***********************************************
static const char* gpGridMapFileNames[] = {
	"assets/map/map_01.txt",

};


//***********************************************
// Mapモデル（テキストデータの１０→１、２０→２）
//***********************************************
enum MAP_MODELID {
	EMPTY, // 0番目は使用しない
	MAP_TREE_01,

	MAX_MAP_MODEL
};


//***********************************************
// FBXモデルのメッシュとテクスチャのファイルパス
//***********************************************
// FBXモデル　メッシュ
static const char* gpFbxMapModelFileNames[] = {
	"assets/model/box_02.fbx", // 0番目は使用しない
	"assets/model/tree_01.fbx",

};

// FBXモデル　テクスチャ
static const char* gpFbxMapTextureFileNames[] = {
	"assets/model/box_02.png", // 0番目は使用しない
	"assets/model/tree_01.png",

};


//*************************************
// Map class
//*************************************
class Map //: public ColliderComponent
{
	//*******************************************
	// 変数
	//*******************************************
private:

	float mGridWidth;         // 1グリッドの横幅
	float mGridHeight;        // 1グリッドの縦幅
	float mLeftX;             // マップの左上座標のX
	float mTopZ;              // マップの左上座標のZ
	XMFLOAT3 mMapTranslation; // マップ全体を原点から動かしたときの移動量（この座標を動かすとマップ全体が移動する）

	GameObject* mMapObject[MAX_MAP_MODEL];
	ColliderComponent* mColliderComponent[MAX_MAP_MODEL];
	int mModelCnt[MAX_MAP_MODEL]; // 各モデルの最大数を保存する
	bool mColliderFlag;           // 当たり判定ありなしフラグ

	//*******************************************
	// 関数
	//*******************************************
public:
	Map();
	~Map();

	// 第一引数：マップのID　第二、三引数：オブジェクト事の配置間隔　第四引数：当たり判定ONOF
	void Init(int, float, float);
	void InitCollider();
	void Update(ColliderComponent*, int id);
	void Draw(bool);
	void Release();

	// 指定したオブジェクトの種類のサイズを変更
	void ScaleSet(int, float, float, float);
	// 

private:
	// オブジェクト配置位置をテキストから読み込み
	HRESULT GridMapLoadFromFile(const char* const pFileName, float, float);
	// オブジェクト一つずつに当たり判定をつける
	void ColliderComponentInit();
	// 指定したColliderComponentが指定したIDのColliderComponentと当たっているか判定
	void ColliderComponentUpdate(ColliderComponent* ,int id);
	void ColliderComponentDraw();
	void ColliderComponentRelease();

};


//*************************************
// 関数
//*************************************
Map* GetGridMap();

void MapInit();
void MapUpdate();
void MapDraw();
void MapRelease();
void MapSetParameter();