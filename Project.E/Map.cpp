#include "Map.h"


//*************************************
// �O���[�o���ϐ�
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
	// �O���b�h�}�b�v�ǂݍ���
	//----------------------------------------------
	hr = GridMapLoadFromFile(gpGridMapFileNames[id], height, width);

	if (FAILED(hr)) {
		//�쐬�Ɏ��s������x�����o���ăv���O�����������I��
		MessageBox(manager->mhWnd, TEXT("GridMapLoadFromFile"), TEXT("ERROR!!"), MB_ICONSTOP);
		PostQuitMessage(0);
	}

}

void Map::InitCollider()
{
	//----------------------------------------------
	// �����蔻��
	//----------------------------------------------
	ColliderComponentInit();
	mColliderFlag = true;
	
}

void Map::Update(ColliderComponent* collider, int id)
{
	//----------------------------------------------
	// �����蔻��
	//----------------------------------------------
	ColliderComponentUpdate(collider, id);

}

void Map::Draw(bool collider)
{
	//----------------------------------------------
	// �O���b�h�}�b�v
	//----------------------------------------------
	for (int i = 1; i < MAX_MAP_MODEL; i++) { // 0�Ԗڂ͕`�悵�Ȃ�
		for (int j = 0; j < mModelCnt[i]; j++) { 
			mMapObject[i][j].Draw();
		}
	}

	//----------------------------------------------
	// �����蔻��
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
	// �O���b�h�}�b�v
	//----------------------------------------------
	for (int i = 1; i < MAX_MAP_MODEL; i++) {
		mMapObject[i]->Release();
		mMapObject[i] = nullptr;
		delete mMapObject[i];
	}

	//----------------------------------------------
	// �����蔻��
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

		int gridNum[GRIDMAP_HEIGHT][GRIDMAP_WIDTH]; // �e�L�X�g�f�[�^�̐��l��ۑ�����

		//---------------------------------------
		// �e�L�X�g�f�[�^����e���f���̐����J�E���g
		//---------------------------------------
		for (int h = 0; h < GRIDMAP_HEIGHT; h++) {
			for (int w = 0; w < GRIDMAP_WIDTH; w++) {
				int num;
				fscanf_s(fp, "%d", &num);  // �^�u��؂�̐����e�L�X�g��ǂݍ���ł���

				gridNum[h][w] = num; // ���l�ۑ�

				int id   = num / 10; // �z�u����I�u�W�F�N�g�̎��
				mModelCnt[id]++;     // �e���f���̐����J�E���g
			}
		}

		//---------------------------------------
		// �J�E���g����K�v�ȃI�u�W�F�N�g�������쐬
		//---------------------------------------
		for(int i = 0; i < MAX_MAP_MODEL; i++) {
			if (i != 0) { // �O�Ԗڂ͉����z�u���Ȃ�
				mMapObject[i] = new GameObject[mModelCnt[i]];
				mMapObject[i][0].FbxInit((char*)gpFbxMapModelFileNames[i], (char*)gpFbxMapTextureFileNames[i]); // FBX���f���ǂݍ���
				for (int j = 0; j < mModelCnt[i]; j++) {
					mMapObject[i][j] = mMapObject[i][0];
					mMapObject[i][j].ShadowOnOf(false);
					mMapObject[i][j].ScaleSet(1.0f, 1.0f, 1.0f);
				}
			}
		}

		//---------------------------------------
		// �쐬�����I�u�W�F�N�g���e�L�X�g�f�[�^�����ɔz�u
		//---------------------------------------
		for (int h = 0; h < GRIDMAP_HEIGHT; h++) {
			for (int w = 0; w < GRIDMAP_WIDTH; w++) {
				
				int id     = gridNum[h][w] / 10; // �z�u����I�u�W�F�N�g�̎��
				float rotY = gridNum[h][w] % 10; // �z�u����I�u�W�F�N�g�̊p�x
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
// �֐�
//*************************************
Map* GetGridMap()
{
	return gpMap;
}

void MapInit()
{
	// ��Ƀ}�b�v�̃p�����[�^�[��ݒ�
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

// �p�����[�^�[�ݒ�
void MapSetParameter()
{
	// ��01
	gMapColliderParameter[TREE_01].aabbLength = { 1.0f, 10.0f, 1.0f };

}