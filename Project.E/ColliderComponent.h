#pragma once
#include "ComponentBase.h"
#include "DirectXMath.h"


using namespace DirectX;


//***********************************************
// Colliderの種類のIDリストを定義する
//***********************************************
enum COLLIDER_TYPE {
	COL_TYPE_SPHERE, // 球
	COL_TYPE_AABB,   // 直方体

	MAX_COL_TYPE
};


//***********************************************
// FBXモデルのメッシュとテクスチャのファイルパス
//***********************************************
// FBXモデル　メッシュ
static const char* gpFbxColliderModelFileNames[] = {
	"assets/model/sphere_01.fbx",
	"assets/model/box_02.fbx",
	
};

// FBXモデル　テクスチャ
static const char* gpFbxColliderTextureFileNames[] = {
	"assets/model/sphere_01.png",
	"assets/model/box_02.png",

};


//***********************************************
// FBXモデルの当たり判定パラメータを設定する構造体
//***********************************************
struct ColliderParameter {
	float sphereRadius;  // オブジェクトのサイズに対しての当たり判定のサイズ倍率
	XMFLOAT3 aabbLength; // オブジェクトのサイズに対しての当たり判定のサイズ倍率
};


//*************************************
// ColliderComponent class
//*************************************
class ColliderComponent : public ComponentBase
{
	//*******************************************
	// 変数
	//*******************************************
protected:
	// 球
	struct sphere {
		XMFLOAT3 centerPos;
		float radius;
	};

	// 直方体
	struct aabb {
		XMFLOAT3 centerPos;
		XMFLOAT3 length;    // 中心点から各頂点までの距離（絶対値）
		XMFLOAT3 rot;
	};

	sphere mSphere;
	aabb mAabb;
	GameObject* pColliderObject;
	XMFLOAT3 mBackupPos;
	int mColliderType;

	//*******************************************
	// 関数
	//*******************************************
public:
	ColliderComponent();
	~ColliderComponent();

	// 第一引数：当たり判定範囲比率（半径）
	void InitSphere(float);
	// 第一引数：当たり判定範囲比率（半径xyz）
	void InitAabb(XMFLOAT3);
	// 第一引数：当たり判定ターゲット
	void Update(ColliderComponent*);
	// バックアップ位置更新（当たり判定確認後に実行する）
	void UpdateBackup();
	void Draw();
	void Release();

	// 球設定
	void SetSphere(XMFLOAT3, float);
	// 直方体設定
	void SetAabb(XMFLOAT3, XMFLOAT3, XMFLOAT3);

	// 衝突判定を返す
	bool Hit(ColliderComponent*);
	bool HitAabb();

};

