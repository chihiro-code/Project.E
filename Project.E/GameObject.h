#pragma once
#include "fbxsdk.h"
#include "DirectX11Manager.h"
#include "dx11texture.h"
#include "DirectXMath.h"


using namespace DirectX;


//***********************************************
// GameObject�ŊǗ����郂�f����ID���X�g���`����
//***********************************************
enum GAMEOBJECT_ID {
	ENEMY_01,   // �S�[�X�g
	TREE_01,    // ��01
	MAP_01,     // �}�b�v01
	SKY_01,     // ��01
	TREASURECHEST, // ��
	BOX_01,     // ��

	MAX_GAMEOBJECT
};


//***********************************************
// FBX���f���̃��b�V���ƃe�N�X�`���̃t�@�C���p�X
//***********************************************
// FBX���f���@���b�V��
static const char* gpFbxModelFileNames[] = {
	"assets/model/Ghost02.fbx",
	"assets/model/tree_01.fbx",
	"assets/model/field_01.fbx",
	"assets/model/sky_01.fbx",
	"assets/model/TreasureChest.fbx",
	"assets/model/box_01.fbx",
};

// FBX���f���@�e�N�X�`��
static const char* gpFbxTextureFileNames[] = {
	"assets/model/Ghost02.png",
	"assets/model/tree_01.png",
	"assets/model/field_01.png",
	"assets/model/sky_01.png",
	"assets/model/TreasureChest.png",
	"assets/model/box_01.png",
};


//*************************************
// GameObject class
//*************************************
class GameObject 
{
	//*******************************************
	// �ϐ�
	//*******************************************
protected:

	//---------------------------------
	// �I�u�W�F�N�g
	//---------------------------------
	
	// �ʒu
	struct Position {
		float x;
		float y;
		float z;
	};

	// ��]
	struct Rotation {
		float x;
		float y;
		float z;
	};

	// �g�k
	struct Scale {
		float x;
		float y;
		float z;
	};

	// �A�j���o�b�t�@
	struct AnimeBuffer {
		XMMATRIX animematrix[128];
	};

	//�{�[���\����
	struct Bone {
		short firstChild;        //�����̎q�{�[���̂����A��ԎႢ�ԍ��̃{�[���ԍ�
		short firstBrother;      //�����Ɠ����e�{�[�������{�[���ԍ�
		XMVECTOR positionVector; //�{�[���̏����ʒu�x�N�g��(x,y,z)�A��]�̏����l��(0,0,0)
		XMMATRIX boneinit;
	};

	// �A�j���[�V�����\����
	struct Animator {
		AnimeBuffer animeBuffer;
		Bone* modelBone;
		XMMATRIX** matrix;
		int startframe;
		int endframe;
	};

	// �ގ��\����
	struct Material {
		float ambient[3];
		float diffuse[3];
		float specular[4];
	};

	// �T�u���b�V���i�I�u�W�F�N�g���ɕ������j
	struct SubMesh {
		ID3D11Buffer* pVertexBuffer; // ���_�o�b�t�@
		ID3D11Buffer* pIndexBuffer;  // �C���f�b�N�X�o�b�t�@
		UINT numIndex;				 // �C���f�b�N�X�f�[�^�̌�
		UINT numVertex;		         // ���_�o�b�t�@�̌�						    
		Material material;			 // �e�ގ��f�[�^							    
		DX11TEXTURE* pTexture;		 // �e�N�X�`���\
	};

	// ���b�V��
	struct Mesh {
		SubMesh* pSubmeshArray; // �T�u���b�V���̔z����m�ۂ����A�h���X������
		Animator animator;      // �A�j���[�V�����Ɋւ���\����
		XMMATRIX baseRot;		// FBX�`�掞�̏����ʒu�̒����p�s��
		UINT numSubmesh;	    // �T�u���b�V���̌�
		UINT modelType;
		int count;
		int boneNum;
		bool motion;			// Motion�̗L�薳��
	};


	//---------------------------------
	// ���f���ǂݍ��݂Ŏg�p��
	//---------------------------------
	
	//�E�F�C�g�ۑ��p�\����
	struct Weight {
#pragma pack(push, 1)
		int bone[4];    //-1�̓G���[�p
		float value[4];
#pragma pack(pop)
	};

	//�|���S�����_�\����
	struct Vertex3D {
#pragma pack(push, 1)
		float pos[3];   //xyz
		float color[4]; //rgb-a
		float normal[3];//xyz
		float tex[2];   //uv
		Weight Vweight; //�e���_�̃E�F�C�g���
#pragma pack(pop)
	};

	//�A�j���[�V�����f�[�^�\����
	struct Anime {
		short targetbone;
		int frame;
		float bonepos[3];
		float bonelotate[4];
		BYTE bezier[4];
	};

	//�A�j���[�V�����\���́@�萔�o�b�t�@�Ƃ��ăV�F�[�_�ɑ��M
	struct LightBuffer {
		XMFLOAT4 objectpos;
		XMFLOAT4 lightpos;
		XMFLOAT4 lightcolor;
		XMFLOAT4 diffuse;
		XMFLOAT4 ambient;
		XMFLOAT4 specular;
		XMFLOAT4 camerapos;
	};

	//���W�ϊ��p�̍\���́@�萔�o�b�t�@�Ƃ��ăV�F�[�_�ɑ��M
	//XMFLOAT4X4 ��DirectXMath.h�Œ�`�����4*4�̍s��
	struct CameraBuffer {
		XMFLOAT4X4 world;            //���[���h�ϊ��s��
		XMFLOAT4X4 view_c;           //�r���[�ϊ��s��i�J�����j
		XMFLOAT4X4 view_l;           //�r���[�ϊ��s��i���C�g�j
		XMFLOAT4X4 projection_c;     //�v���W�F�N�V�����ϊ��s��i�J�����j
		XMFLOAT4X4 projection_l;     //�v���W�F�N�V�����ϊ��s��i���C�g�j
	};

protected:
	Mesh* mMeshData;
	Position mPos;
	Rotation mRot;
	Scale mScl;

	bool mDrawFlag;		// �`�� ON OF
	bool mDrawShadow;   // �e�̕`�� ON OF
	bool mAlive;        // �����t���O


	//---------------------------------
	// ���f���ǂݍ��݂Ŏg�p��
	//---------------------------------
	
	//�ǂݍ��ݗp�ꎞ�ϐ�
	int mFrame;
	FbxTime mStartTime;
	FbxTime mEndTime;
	FbxTime mFrameTime;

	//���C�e�B���O�p
	LightBuffer mLbuf;

	
	//*******************************************
	// �֐�
	//*******************************************
public:
	GameObject();
	~GameObject();

	//---------------------------------
	// �I�u�W�F�N�g
	//---------------------------------

	// �����P�F���f��ID�@�����Q�F�e�N�X�`��ID
	void Init(int modelId, int texId);
	void Update(void);
	void Draw(void);
	void Release(void);
	void PositionSet(float x, float y, float z);
	void RotationSet(float x, float y, float z);
	void ScaleSet(float x, float y, float z);
	void SpecularSet(float red, float green, float blue, float power);
	void ShadowOnOf(bool);
	void ShadowDraw(); // �`��i�e�j

	XMFLOAT3 PositionGet();
	XMFLOAT3 RotationGet();
	XMFLOAT3 ScaleGet();
	


private:
	//---------------------------------
	// ���f���ǂݍ��݂Ŏg�p��
	//---------------------------------

	// ������
	void FbxInit(char*, char*);

	// �V�F�[�_�[�����������[�V�������肩�Ȃ��ŕ��򂳂���
	void FbxShaderInitChoice(Mesh*);

	// �V�F�[�_�[�`��i���[�V��������j
	void FbxShaderDraw(CameraBuffer);

	// �V�F�[�_�[�`��i���[�V�����Ȃ��j
	void FbxNoMotionShaderDraw(CameraBuffer);

	// �`��
	void FbxDraw();

	//fbx�t�@�C���̃��[�_�[�֐��̑O���錾
	void FbxLoader(const char[]);

	//fbx�̃m�[�h����������֐��̑O���錾
	void GetMeshData(int, FbxScene*);

	//fbx�A�j���[�V�����֐��̑O���錾
	void FbxSetPose(int);

	//fbx����ގ��f�[�^�i�����o�[�g�j�𒊏o
	void GetMaterialLambert(SubMesh, FbxSurfaceMaterial*);


	//---------------------------------
	// �e�\���Ŏg�p��
	//---------------------------------

	// �V�F�[�_�[�`��i�e�j
	void ShadowShaderDraw(CameraBuffer);
	
	// ��ʃN���A�i�e�j
	void ShadowClearScreen();


};


//*************************************
// �֐�
//*************************************

// �Q�b�g�֐�
// ID���w�肵�ăI�u�W�F�N�g���擾
GameObject* GetGameObject(int);

void GameObjectInit();
void GameObjectUpdate();
void GameObjectDraw();
void GameObjectRelease();
void GameObjectSetParameter();
