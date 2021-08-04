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
	COL_TYPE_POINT,  // 点

	MAX_COL_TYPE
};


//***********************************************
// FBXモデルのメッシュとテクスチャのファイルパス
//***********************************************
// FBXモデル　メッシュ
static const char* gpFbxColliderModelFileNames[] = {
	"assets/model/Ghost02.fbx",
	"assets/model/tree_01.fbx",
	"assets/model/field_01.fbx",
	"assets/model/sky_01.fbx",
	"assets/model/TreasureChest.fbx",
};

// FBXモデル　テクスチャ
static const char* gpFbxColliderTextureFileNames[] = {
	"assets/model/Ghost02.png",
	"assets/model/tree_01.png",
	"assets/model/field_01.png",
	"assets/model/sky_01.png",
	"assets/model/TreasureChest.png",
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


	//*******************************************
	// 関数
	//*******************************************
public:
	ColliderComponent();
	~ColliderComponent();

	void Init() override;
	void Update() override;
	void Release() override;

	// 球設定
	void SetSphere(XMFLOAT3, float);
	// 直方体設定
	void SetAabb(XMFLOAT3, XMFLOAT3, XMFLOAT3);

	// 衝突判定を返す
	bool GetSphere();
	bool GetAabb();

};

