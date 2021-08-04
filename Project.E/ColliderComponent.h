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
	COL_TYPE_POINT,  // �_

	MAX_COL_TYPE
};


//***********************************************
// FBX���f���̃��b�V���ƃe�N�X�`���̃t�@�C���p�X
//***********************************************
// FBX���f���@���b�V��
static const char* gpFbxColliderModelFileNames[] = {
	"assets/model/Ghost02.fbx",
	"assets/model/tree_01.fbx",
	"assets/model/field_01.fbx",
	"assets/model/sky_01.fbx",
	"assets/model/TreasureChest.fbx",
};

// FBX���f���@�e�N�X�`��
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


	//*******************************************
	// �֐�
	//*******************************************
public:
	ColliderComponent();
	~ColliderComponent();

	void Init() override;
	void Update() override;
	void Release() override;

	// ���ݒ�
	void SetSphere(XMFLOAT3, float);
	// �����̐ݒ�
	void SetAabb(XMFLOAT3, XMFLOAT3, XMFLOAT3);

	// �Փ˔����Ԃ�
	bool GetSphere();
	bool GetAabb();

};

