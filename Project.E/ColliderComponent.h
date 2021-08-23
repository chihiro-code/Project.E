#pragma once
#include "ComponentBase.h"
#include "DirectXMath.h"


using namespace DirectX;


//***********************************************
// Collider�̎�ނ�ID���X�g���`����
//***********************************************
enum COLLIDER_TYPE {
	COL_TYPE_SPHERE, // ��
	COL_TYPE_AABB,   // ������

	MAX_COL_TYPE
};


//***********************************************
// FBX���f���̃��b�V���ƃe�N�X�`���̃t�@�C���p�X
//***********************************************
// FBX���f���@���b�V��
static const char* gpFbxColliderModelFileNames[] = {
	"assets/model/sphere_01.fbx",
	"assets/model/box_02.fbx",
	
};

// FBX���f���@�e�N�X�`��
static const char* gpFbxColliderTextureFileNames[] = {
	"assets/model/sphere_01.png",
	"assets/model/box_02.png",

};


//***********************************************
// FBX���f���̓����蔻��p�����[�^��ݒ肷��\����
//***********************************************
struct ColliderParameter {
	float sphereRadius;  // �I�u�W�F�N�g�̃T�C�Y�ɑ΂��Ă̓����蔻��̃T�C�Y�{��
	XMFLOAT3 aabbLength; // �I�u�W�F�N�g�̃T�C�Y�ɑ΂��Ă̓����蔻��̃T�C�Y�{��
};


//*************************************
// ColliderComponent class
//*************************************
class ColliderComponent : public ComponentBase
{
	//*******************************************
	// �ϐ�
	//*******************************************
protected:
	// ��
	struct sphere {
		XMFLOAT3 centerPos;
		float radius;
	};

	// ������
	struct aabb {
		XMFLOAT3 centerPos;
		XMFLOAT3 length;    // ���S�_����e���_�܂ł̋����i��Βl�j
		XMFLOAT3 rot;
	};

	sphere mSphere;
	aabb mAabb;
	GameObject* pColliderObject;
	XMFLOAT3 mBackupPos;
	int mColliderType;

	//*******************************************
	// �֐�
	//*******************************************
public:
	ColliderComponent();
	~ColliderComponent();

	// �������F�����蔻��͈͔䗦�i���a�j
	void InitSphere(float);
	// �������F�����蔻��͈͔䗦�i���axyz�j
	void InitAabb(XMFLOAT3);
	// �������F�����蔻��^�[�Q�b�g
	void Update(ColliderComponent*);
	// �o�b�N�A�b�v�ʒu�X�V�i�����蔻��m�F��Ɏ��s����j
	void UpdateBackup();
	void Draw();
	void Release();

	// ���ݒ�
	void SetSphere(XMFLOAT3, float);
	// �����̐ݒ�
	void SetAabb(XMFLOAT3, XMFLOAT3, XMFLOAT3);

	// �Փ˔����Ԃ�
	bool Hit(ColliderComponent*);
	bool HitAabb();

};

