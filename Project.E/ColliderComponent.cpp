#include "ColliderComponent.h"


//*************************************
// �O���[�o���ϐ�
//*************************************


//*************************************
// ColliderComponent�֐�
//*************************************
ColliderComponent::ColliderComponent()
{
	// ��
	mSphere.centerPos = { 0,0,0 };
	mSphere.radius = 0;

	// ������
	mAabb.centerPos = { 0,0,0 };
	mAabb.length = { 0,0,0 };
	mAabb.rot = { 0,0,0 };

	// �o�b�N�A�b�v�ʒu
	mBackupPos = { 0,0,0 };

	// �����蔻��^�C�v
	mColliderType = 0;
}

ColliderComponent::~ColliderComponent()
{
	delete pColliderObject;
	
}

void ColliderComponent::InitSphere(float radius)
{
	// �ʒu�A�͈͐ݒ�
	mSphere.centerPos = mpOwner->PositionGet();
	mSphere.radius = radius;

	// �o�b�N�A�b�v�ʒu�ۑ�
	mBackupPos = mSphere.centerPos;

	// �����蔻��^�C�v
	mColliderType = COL_TYPE_SPHERE;

	// �����蔻����������邽�߂̃I�u�W�F�N�g�쐬
	pColliderObject = new GameObject;
	pColliderObject->FbxInit((char*)gpFbxColliderModelFileNames[COL_TYPE_SPHERE], (char*)gpFbxColliderTextureFileNames[COL_TYPE_SPHERE]);
	pColliderObject->ShadowOnOf(false);
	pColliderObject->PositionSet(mSphere.centerPos.x, mSphere.centerPos.y, mSphere.centerPos.z);
	pColliderObject->RotationSet(0, 0, 0);
	pColliderObject->ScaleSet(0.1f * radius, 0.1f * radius, 0.1f * radius);

}

void ColliderComponent::InitAabb(XMFLOAT3 length)
{
	// �ʒu�A�͈͐ݒ�
	mAabb.centerPos = mpOwner->PositionGet();
	mAabb.rot = mpOwner->RotationGet();
	mAabb.length = length;

	// �o�b�N�A�b�v�ʒu�ۑ�
	mBackupPos = mAabb.centerPos;

	// �����蔻��^�C�v
	mColliderType = COL_TYPE_AABB;

	// �����蔻����������邽�߂̃I�u�W�F�N�g�쐬
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
	// �����̓����蔻��̃^�C�v�ɂ���ď�����ς���
	//-----------------------------------------------------

	switch (mColliderType) {

	case COL_TYPE_SPHERE: // �� 
		// �����蔻��ʒu�X�V
		mSphere.centerPos = mpOwner->PositionGet();
		// ���f���̈ʒu�X�V
		pColliderObject->PositionSet(mSphere.centerPos.x, mSphere.centerPos.y, mSphere.centerPos.z);
		break;

	case COL_TYPE_AABB: // ������ 
		// �����蔻��ʒu�X�V
		mAabb.centerPos = mpOwner->PositionGet();
		// ���f���̈ʒu�X�V
		pColliderObject->PositionSet(mAabb.centerPos.x, mAabb.centerPos.y, mAabb.centerPos.z);
		break;

	}

	// �^�[�Q�b�g�ɏՓ˂��Ă���ΏՓˌ�̏������s
	if (Hit(pColTarget)) {
		// �ʒu��߂�
		mpOwner->PositionSet(mBackupPos.x, mBackupPos.y, mBackupPos.z);
	}
	
}

void ColliderComponent::UpdateBackup()
{
	// �o�b�N�A�b�v�ʒu�X�V
	mBackupPos = mpOwner->PositionGet();
}

void ColliderComponent::Draw()
{
	// ���f���`��
	pColliderObject->Draw();
}

void ColliderComponent::Release()
{
	
}

void ColliderComponent::SetSphere(XMFLOAT3 centerPos, float radius)
{
	// �����蔻��ʒu�A�͈͐ݒ�
	mSphere.centerPos = mpOwner->PositionGet();
	mSphere.radius = radius;

	// �����蔻��^�C�v�ݒ�
	mColliderType = COL_TYPE_SPHERE;

	// ���f���̈ʒu�ύX
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
	// �����ƃ^�[�Q�b�g�̓����蔻��̃^�C�v�ɂ���ď�����ς���
	//-----------------------------------------------------
	float distanceX;
	float distanceY;
	float distanceZ;
	float sumedRadius;
	float sumedRadiusX;
	float sumedRadiusY;
	float sumedRadiusZ;

	// �����̃^�C�v
	switch (mColliderType) {

	case COL_TYPE_SPHERE: // �� ===================================================================

		// �^�[�Q�b�g�̃^�C�v
		switch (pColTarget->mColliderType) {

		case COL_TYPE_SPHERE: // VS�� -------------------------------------------------------------

			// ��̃I�u�W�F�N�g���������߂�
			distanceX = mSphere.centerPos.x - pColTarget->mSphere.centerPos.x;
			distanceY = mSphere.centerPos.y - pColTarget->mSphere.centerPos.y;
			distanceZ = mSphere.centerPos.z - pColTarget->mSphere.centerPos.z;

			// ���̔��a�{���̔��a
			sumedRadius = mSphere.radius + pColTarget->mSphere.radius;

			// �I�u�W�F�N�g�����Ɣ��a�̍��v�œ������Ă��邩����
			return distanceX * distanceX + distanceY * distanceY + distanceZ * distanceZ <= sumedRadius * sumedRadius;
			break;

		case COL_TYPE_AABB: // VS������ ------------------------------------------------------------

			// ��̃I�u�W�F�N�g���������߂�
			distanceX = fabsf(mSphere.centerPos.x - pColTarget->mAabb.centerPos.x);
			distanceY = fabsf(mSphere.centerPos.y - pColTarget->mAabb.centerPos.y);
			distanceZ = fabsf(mSphere.centerPos.z - pColTarget->mAabb.centerPos.z);

			// ���̔��a�{�����̂̊e���a
			sumedRadiusX = mSphere.radius + pColTarget->mAabb.length.x;
			sumedRadiusY = mSphere.radius + pColTarget->mAabb.length.y;
			sumedRadiusZ = mSphere.radius + pColTarget->mAabb.length.z;

			// �I�u�W�F�N�g�����Ɣ��a�̍��v�œ������Ă��邩����
			return distanceX <= sumedRadiusX && distanceY <= sumedRadiusY && distanceZ <= sumedRadiusZ;
			break;

		}
		break;

	case COL_TYPE_AABB: // ������ ==================================================================

		// �^�[�Q�b�g�̃^�C�v
		switch (pColTarget->mColliderType) {

		case COL_TYPE_SPHERE: // VS�� -------------------------------------------------------------

			// ��̃I�u�W�F�N�g���������߂�
			distanceX = mAabb.centerPos.x - pColTarget->mSphere.centerPos.x;
			distanceY = mAabb.centerPos.y - pColTarget->mSphere.centerPos.y;
			distanceZ = mAabb.centerPos.z - pColTarget->mSphere.centerPos.z;

			// �����̂̊e���a�{���̔��a
			sumedRadiusX = mAabb.length.x + pColTarget->mSphere.radius;
			sumedRadiusY = mAabb.length.y + pColTarget->mSphere.radius;
			sumedRadiusZ = mAabb.length.z + pColTarget->mSphere.radius;

			// �I�u�W�F�N�g�����Ɣ��a�̍��v�œ������Ă��邩����
			return distanceX <= sumedRadiusX && distanceY <= sumedRadiusY && distanceZ <= sumedRadiusZ;
			break;

		case COL_TYPE_AABB: // VS������ ------------------------------------------------------------

			// ��̃I�u�W�F�N�g���������߂�
			distanceX = fabsf(mAabb.centerPos.x - pColTarget->mAabb.centerPos.x);
			distanceY = fabsf(mAabb.centerPos.y - pColTarget->mAabb.centerPos.y);
			distanceZ = fabsf(mAabb.centerPos.z - pColTarget->mAabb.centerPos.z);

			// �����̂̊e���a�{�����̂̊e���a
			sumedRadiusX = mAabb.length.x + pColTarget->mAabb.length.x;
			sumedRadiusY = mAabb.length.y + pColTarget->mAabb.length.y;
			sumedRadiusZ = mAabb.length.z + pColTarget->mAabb.length.z;

			// �I�u�W�F�N�g�����Ɣ��a�̍��v�œ������Ă��邩����
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



