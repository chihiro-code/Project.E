#include "Map.h"


//*************************************
// グローバル変数
//*************************************
Map* gpMap;
ColliderParameter gMapColliderParameter[MAX_MAP_MODEL];


Map::Map()
{
	mGridHeight = 50.0f;
	mGridWidth  = 50.0f;
	mLeftX = 0.0f;
	mTopZ  = 0.0f;
	mMapTranslation.x = 0.0f;
	mMapTranslation.y = 0.0f;
	mMapTranslation.z = 0.0f;
	mColliderFlag = false;

	for (int i = 0; i < MAX_MAP_MODEL; i++) {
		mModelCnt[i] = 0;
	}
}

Map::~Map()
{
	
}

void Map::Init(int id, float height, float width)
{
	DirectX11Manager* manager = GetDirectX11Manager();
	HRESULT hr;

	//----------------------------------------------
	// グリッドマップ読み込み
	//----------------------------------------------
	hr = GridMapLoadFromFile(gpGridMapFileNames[id], height, width);

	if (FAILED(hr)) {
		//作成に失敗したら警告を出してプログラムを強制終了
		MessageBox(manager->mhWnd, TEXT("GridMapLoadFromFile"), TEXT("ERROR!!"), MB_ICONSTOP);
		PostQuitMessage(0);
	}

}

void Map::InitCollider()
{
	//----------------------------------------------
	// 当たり判定
	//----------------------------------------------
	ColliderComponentInit();
	mColliderFlag = true;
	
}

void Map::Update(ColliderComponent* collider, int id)
{
	//----------------------------------------------
	// 当たり判定
	//----------------------------------------------
	ColliderComponentUpdate(collider, id);

}

void Map::Draw(bool collider)
{
	//----------------------------------------------
	// グリッドマップ
	//----------------------------------------------
	for (int i = 1; i < MAX_MAP_MODEL; i++) { // 0番目は描画しない
		for (int j = 0; j < mModelCnt[i]; j++) { 
			mMapObject[i][j].Draw();
		}
	}

	//----------------------------------------------
	// 当たり判定
	//----------------------------------------------
	if (collider == true) {
		if (mColliderFlag == true) {
			ColliderComponentDraw();
		}
	}

}

void Map::Release()
{
	//----------------------------------------------
	// グリッドマップ
	//----------------------------------------------
	for (int i = 1; i < MAX_MAP_MODEL; i++) {
		mMapObject[i]->Release();
		mMapObject[i] = nullptr;
		delete mMapObject[i];
	}

	//----------------------------------------------
	// 当たり判定
	//----------------------------------------------
	if (mColliderFlag == true) {
		ColliderComponentRelease();
	}

}

void Map::ScaleSet(int id, float sclX, float sclY, float sclZ)
{
	for (int i = 0; i < mModelCnt[id]; i++) {
		mMapObject[id][i].ScaleSet(sclX, sclY, sclZ);
	}
}

HRESULT Map::GridMapLoadFromFile(const char* const pFileName, float height, float width)
{
	FILE* fp = nullptr;

	if (fopen_s(&fp, pFileName, "rt") == 0) {
		float originX = width  * GRIDMAP_WIDTH / -2.0f;
		float originZ = height * GRIDMAP_HEIGHT / 2.0f;

		mLeftX = originX;
		mTopZ  = originZ;

		int cnt[MAX_MAP_MODEL];
		for (int i = 0; i < MAX_MAP_MODEL; i++) {
			cnt[i] = 0;
		}

		int gridNum[GRIDMAP_HEIGHT][GRIDMAP_WIDTH]; // テキストデータの数値を保存する

		//---------------------------------------
		// テキストデータから各モデルの数をカウント
		//---------------------------------------
		for (int h = 0; h < GRIDMAP_HEIGHT; h++) {
			for (int w = 0; w < GRIDMAP_WIDTH; w++) {
				int num;
				fscanf_s(fp, "%d", &num);  // タブ区切りの数字テキストを読み込んでいく

				gridNum[h][w] = num; // 数値保存

				int id   = num / 10; // 配置するオブジェクトの種類
				mModelCnt[id]++;     // 各モデルの数をカウント
			}
		}

		//---------------------------------------
		// カウントから必要なオブジェクト数だけ作成
		//---------------------------------------
		for(int i = 0; i < MAX_MAP_MODEL; i++) {
			if (i != 0) { // ０番目は何も配置しない
				mMapObject[i] = new GameObject[mModelCnt[i]];
				mMapObject[i][0].FbxInit((char*)gpFbxMapModelFileNames[i], (char*)gpFbxMapTextureFileNames[i]); // FBXモデル読み込み
				for (int j = 0; j < mModelCnt[i]; j++) {
					mMapObject[i][j] = mMapObject[i][0];
					mMapObject[i][j].ShadowOnOf(false);
					mMapObject[i][j].ScaleSet(1.0f, 1.0f, 1.0f);
				}
			}
		}

		//---------------------------------------
		// 作成したオブジェクトをテキストデータを元に配置
		//---------------------------------------
		for (int h = 0; h < GRIDMAP_HEIGHT; h++) {
			for (int w = 0; w < GRIDMAP_WIDTH; w++) {
				
				int id     = gridNum[h][w] / 10; // 配置するオブジェクトの種類
				float rotY = gridNum[h][w] % 10; // 配置するオブジェクトの角度
				float posX = 0.0f;
				float posY = 0.0f;
				float posZ = 0.0f;

				if (id != 0) {
					posX = originX + width  * w + width  / 2.0f + mMapTranslation.x;
					posZ = originZ - height * h - height / 2.0f + mMapTranslation.z;
					posY = 0.0f;
					mMapObject[id][cnt[id]].RotationSet(0.0f, rotY * 90.0f, 0.0f);
					mMapObject[id][cnt[id]].PositionSet(posX, posY, posZ);
					cnt[id]++;
				}
			}
		}

		fclose(fp);
		return S_OK;
	}

	return E_FAIL;
}

void Map::ColliderComponentInit()
{
	for (int i = 1; i < MAX_MAP_MODEL; i++) {
		mColliderComponent[i] = new ColliderComponent[mModelCnt[i]];
		for (int j = 0; j < mModelCnt[i]; j++) {
			mColliderComponent[i][j].SetOwner(&mMapObject[i][j]);
			mColliderComponent[i][j].InitAabb(gMapColliderParameter[i].aabbLength);
		}
	}

}

void Map::ColliderComponentUpdate(ColliderComponent* component, int id)
{
	for (int i = 0; i < mModelCnt[id]; i++) {
		component->Update(&mColliderComponent[id][i]);
	}
	
}

void Map::ColliderComponentDraw()
{
	for (int i = 1; i < MAX_MAP_MODEL; i++) {
		for (int j = 0; j < mModelCnt[i]; j++) {
			mColliderComponent[i][j].Draw();
		}
	}
}

void Map::ColliderComponentRelease()
{
	for (int i = 1; i < MAX_MAP_MODEL; i++) {
		mColliderComponent[i]->Release();
		mColliderComponent[i] = nullptr;
		delete mColliderComponent[i];
	}
}


//*************************************
// 関数
//*************************************
Map* GetGridMap()
{
	return gpMap;
}

void MapInit()
{
	// 先にマップのパラメーターを設定
	MapSetParameter();

	gpMap = new Map;
	gpMap->Init(GRIDMAP_01, 50.0f, 50.0f);
	gpMap->ScaleSet(MAP_TREE_01, 5.0f, 5.0f, 5.0f);
	gpMap->InitCollider();

}

void MapUpdate()
{
	
}

void MapDraw()
{
	gpMap->Draw(true);

}

void MapRelease()
{
	gpMap->Release();
	delete gpMap;

}

// パラメーター設定
void MapSetParameter()
{
	// 木01
	gMapColliderParameter[TREE_01].aabbLength = { 1.0f, 10.0f, 1.0f };

}