#pragma once
#include "ColliderComponent.h"


//***********************************************
// �O���b�g�}�b�v�̑傫�����`�i�e�L�X�g�f�[�^�̏c���̐��ƍ��킹��j
//***********************************************
#define GRIDMAP_WIDTH  50
#define GRIDMAP_HEIGHT 50


//***********************************************
// Map�̎�ނ�ID���X�g���`����
//***********************************************
enum MAP_LIST {
	GRIDMAP_01,

	MAX_MAP_LIST
};


//***********************************************
// Map�z�u�̃e�L�X�g�f�[�^�̃t�@�C���p�X
//***********************************************
static const char* gpGridMapFileNames[] = {
	"assets/map/map_01.txt",

};


//***********************************************
// Map���f���i�e�L�X�g�f�[�^�̂P�O���P�A�Q�O���Q�j
//***********************************************
enum MAP_MODELID {
	EMPTY, // 0�Ԗڂ͎g�p���Ȃ�
	MAP_TREE_01,

	MAX_MAP_MODEL
};


//***********************************************
// FBX���f���̃��b�V���ƃe�N�X�`���̃t�@�C���p�X
//***********************************************
// FBX���f���@���b�V��
static const char* gpFbxMapModelFileNames[] = {
	"assets/model/box_02.fbx", // 0�Ԗڂ͎g�p���Ȃ�
	"assets/model/tree_01.fbx",

};

// FBX���f���@�e�N�X�`��
static const char* gpFbxMapTextureFileNames[] = {
	"assets/model/box_02.png", // 0�Ԗڂ͎g�p���Ȃ�
	"assets/model/tree_01.png",

};


//*************************************
// Map class
//*************************************
class Map //: public ColliderComponent
{
	//*******************************************
	// �ϐ�
	//*******************************************
private:

	float mGridWidth;         // 1�O���b�h�̉���
	float mGridHeight;        // 1�O���b�h�̏c��
	float mLeftX;             // �}�b�v�̍�����W��X
	float mTopZ;              // �}�b�v�̍�����W��Z
	XMFLOAT3 mMapTranslation; // �}�b�v�S�̂����_���瓮�������Ƃ��̈ړ��ʁi���̍��W�𓮂����ƃ}�b�v�S�̂��ړ�����j

	GameObject* mMapObject[MAX_MAP_MODEL];
	ColliderComponent* mColliderComponent[MAX_MAP_MODEL];
	int mModelCnt[MAX_MAP_MODEL]; // �e���f���̍ő吔��ۑ�����
	bool mColliderFlag;           // �����蔻�肠��Ȃ��t���O

	//*******************************************
	// �֐�
	//*******************************************
public:
	Map();
	~Map();

	// �������F�}�b�v��ID�@���A�O�����F�I�u�W�F�N�g���̔z�u�Ԋu�@��l�����F�����蔻��ONOF
	void Init(int, float, float);
	void InitCollider();
	void Update(ColliderComponent*, int id);
	void Draw(bool);
	void Release();

	// �w�肵���I�u�W�F�N�g�̎�ނ̃T�C�Y��ύX
	void ScaleSet(int, float, float, float);
	// 

private:
	// �I�u�W�F�N�g�z�u�ʒu���e�L�X�g����ǂݍ���
	HRESULT GridMapLoadFromFile(const char* const pFileName, float, float);
	// �I�u�W�F�N�g����ɓ����蔻�������
	void ColliderComponentInit();
	// �w�肵��ColliderComponent���w�肵��ID��ColliderComponent�Ɠ������Ă��邩����
	void ColliderComponentUpdate(ColliderComponent* ,int id);
	void ColliderComponentDraw();
	void ColliderComponentRelease();

};


//*************************************
// �֐�
//*************************************
Map* GetGridMap();

void MapInit();
void MapUpdate();
void MapDraw();
void MapRelease();
void MapSetParameter();