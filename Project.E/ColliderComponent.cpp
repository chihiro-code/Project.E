#include "ColliderComponent.h"


//*************************************
// グローバル変数
//*************************************


//*************************************
// ColliderComponent関数
//*************************************
ColliderComponent::ColliderComponent()
{
	// 球
	mSphere.centerPos = { 0,0,0 };
	mSphere.radius = 0;

	// 直方体
	mAabb.centerPos = { 0,0,0 };
	mAabb.length = { 0,0,0 };
	mAabb.rot = { 0,0,0 };

	// バックアップ位置
	mBackupPos = { 0,0,0 };

	// 当たり判定タイプ
	mColliderType = 0;
}

ColliderComponent::~ColliderComponent()
{
	delete pColliderObject;
	
}

void ColliderComponent::InitSphere(float radius)
{
	// 位置、範囲設定
	mSphere.centerPos = mpOwner->PositionGet();
	mSphere.radius = radius;

	// バックアップ位置保存
	mBackupPos = mSphere.centerPos;

	// 当たり判定タイプ
	mColliderType = COL_TYPE_SPHERE;

	// 当たり判定を可視化するためのオブジェクト作成
	pColliderObject = new GameObject;
	pColliderObject->FbxInit((char*)gpFbxColliderModelFileNames[COL_TYPE_SPHERE], (char*)gpFbxColliderTextureFileNames[COL_TYPE_SPHERE]);
	pColliderObject->ShadowOnOf(false);
	pColliderObject->PositionSet(mSphere.centerPos.x, mSphere.centerPos.y, mSphere.centerPos.z);
	pColliderObject->RotationSet(0, 0, 0);
	pColliderObject->ScaleSet(0.1f * radius, 0.1f * radius, 0.1f * radius);

}

void ColliderComponent::InitAabb(XMFLOAT3 length)
{
	// 位置、範囲設定
	mAabb.centerPos = mpOwner->PositionGet();
	mAabb.rot = mpOwner->RotationGet();
	mAabb.length = length;

	// バックアップ位置保存
	mBackupPos = mAabb.centerPos;

	// 当たり判定タイプ
	mColliderType = COL_TYPE_AABB;

	// 当たり判定を可視化するためのオブジェクト作成
	pColliderObject = new GameObject;
	pColliderObject->FbxInit((char*)gpFbxColliderModelFileNames[COL_TYPE_AABB], (char*)gpFbxColliderTextureFileNames[COL_TYPE_AABB]);
	pColliderObject->ShadowOnOf(false);
	pColliderObject->PositionSet(mAabb.centerPos.x, mAabb.centerPos.y, mAabb.centerPos.z);
	pColliderObject->RotationSet(mAabb.rot.x, mAabb.rot.y, mAabb.rot.z);
	pColliderObject->ScaleSet(0.1f * length.x, 0.1f * length.y, 0.1f * length.z);
}

void ColliderComponent::Update(ColliderComponent* pColTarget)
{
	//-----------------------------------------------------
	// 自分の当たり判定のタイプによって処理を変える
	//-----------------------------------------------------

	switch (mColliderType) {

	case COL_TYPE_SPHERE: // 球 
		// 当たり判定位置更新
		mSphere.centerPos = mpOwner->PositionGet();
		// モデルの位置更新
		pColliderObject->PositionSet(mSphere.centerPos.x, mSphere.centerPos.y, mSphere.centerPos.z);
		break;

	case COL_TYPE_AABB: // 直方体 
		// 当たり判定位置更新
		mAabb.centerPos = mpOwner->PositionGet();
		// モデルの位置更新
		pColliderObject->PositionSet(mAabb.centerPos.x, mAabb.centerPos.y, mAabb.centerPos.z);
		break;

	}

	// ターゲットに衝突していれば衝突後の処理実行
	if (Hit(pColTarget)) {
		// 位置を戻す
		mpOwner->PositionSet(mBackupPos.x, mBackupPos.y, mBackupPos.z);
	}
	
}

void ColliderComponent::UpdateBackup()
{
	// バックアップ位置更新
	mBackupPos = mpOwner->PositionGet();
}

void ColliderComponent::Draw()
{
	// モデル描画
	pColliderObject->Draw();
}

void ColliderComponent::Release()
{
	
}

void ColliderComponent::SetSphere(XMFLOAT3 centerPos, float radius)
{
	// 当たり判定位置、範囲設定
	mSphere.centerPos = mpOwner->PositionGet();
	mSphere.radius = radius;

	// 当たり判定タイプ設定
	mColliderType = COL_TYPE_SPHERE;

	// モデルの位置変更
	pColliderObject->PositionSet(mSphere.centerPos.x, mSphere.centerPos.y, mSphere.centerPos.z);

}

void ColliderComponent::SetAabb(XMFLOAT3 centerPos, XMFLOAT3 length, XMFLOAT3 rot)
{
	mAabb.centerPos = centerPos;
	mAabb.length = length;
	mAabb.rot = rot;
}

bool ColliderComponent::Hit(ColliderComponent* pColTarget)
{
	//-----------------------------------------------------
	// 自分とターゲットの当たり判定のタイプによって処理を変える
	//-----------------------------------------------------
	float distanceX;
	float distanceY;
	float distanceZ;
	float sumedRadius;
	float sumedRadiusX;
	float sumedRadiusY;
	float sumedRadiusZ;

	// 自分のタイプ
	switch (mColliderType) {

	case COL_TYPE_SPHERE: // 球 ===================================================================

		// ターゲットのタイプ
		switch (pColTarget->mColliderType) {

		case COL_TYPE_SPHERE: // VS球 -------------------------------------------------------------

			// 二つのオブジェクト距離を求める
			distanceX = mSphere.centerPos.x - pColTarget->mSphere.centerPos.x;
			distanceY = mSphere.centerPos.y - pColTarget->mSphere.centerPos.y;
			distanceZ = mSphere.centerPos.z - pColTarget->mSphere.centerPos.z;

			// 球の半径＋球の半径
			sumedRadius = mSphere.radius + pColTarget->mSphere.radius;

			// オブジェクト距離と半径の合計で当たっているか判定
			return distanceX * distanceX + distanceY * distanceY + distanceZ * distanceZ <= sumedRadius * sumedRadius;
			break;

		case COL_TYPE_AABB: // VS直方体 ------------------------------------------------------------

			// 二つのオブジェクト距離を求める
			distanceX = fabsf(mSphere.centerPos.x - pColTarget->mAabb.centerPos.x);
			distanceY = fabsf(mSphere.centerPos.y - pColTarget->mAabb.centerPos.y);
			distanceZ = fabsf(mSphere.centerPos.z - pColTarget->mAabb.centerPos.z);

			// 球の半径＋直方体の各半径
			sumedRadiusX = mSphere.radius + pColTarget->mAabb.length.x;
			sumedRadiusY = mSphere.radius + pColTarget->mAabb.length.y;
			sumedRadiusZ = mSphere.radius + pColTarget->mAabb.length.z;

			// オブジェクト距離と半径の合計で当たっているか判定
			return distanceX <= sumedRadiusX && distanceY <= sumedRadiusY && distanceZ <= sumedRadiusZ;
			break;

		}
		break;

	case COL_TYPE_AABB: // 直方体 ==================================================================

		// ターゲットのタイプ
		switch (pColTarget->mColliderType) {

		case COL_TYPE_SPHERE: // VS球 -------------------------------------------------------------

			// 二つのオブジェクト距離を求める
			distanceX = mAabb.centerPos.x - pColTarget->mSphere.centerPos.x;
			distanceY = mAabb.centerPos.y - pColTarget->mSphere.centerPos.y;
			distanceZ = mAabb.centerPos.z - pColTarget->mSphere.centerPos.z;

			// 直方体の各半径＋球の半径
			sumedRadiusX = mAabb.length.x + pColTarget->mSphere.radius;
			sumedRadiusY = mAabb.length.y + pColTarget->mSphere.radius;
			sumedRadiusZ = mAabb.length.z + pColTarget->mSphere.radius;

			// オブジェクト距離と半径の合計で当たっているか判定
			return distanceX <= sumedRadiusX && distanceY <= sumedRadiusY && distanceZ <= sumedRadiusZ;
			break;

		case COL_TYPE_AABB: // VS直方体 ------------------------------------------------------------

			// 二つのオブジェクト距離を求める
			distanceX = fabsf(mAabb.centerPos.x - pColTarget->mAabb.centerPos.x);
			distanceY = fabsf(mAabb.centerPos.y - pColTarget->mAabb.centerPos.y);
			distanceZ = fabsf(mAabb.centerPos.z - pColTarget->mAabb.centerPos.z);

			// 直方体の各半径＋直方体の各半径
			sumedRadiusX = mAabb.length.x + pColTarget->mAabb.length.x;
			sumedRadiusY = mAabb.length.y + pColTarget->mAabb.length.y;
			sumedRadiusZ = mAabb.length.z + pColTarget->mAabb.length.z;

			// オブジェクト距離と半径の合計で当たっているか判定
			return distanceX <= sumedRadiusX && distanceY <= sumedRadiusY && distanceZ <= sumedRadiusZ;
			break;

		}
		break;

	}

}

bool ColliderComponent::HitAabb()
{
	return false;
}



