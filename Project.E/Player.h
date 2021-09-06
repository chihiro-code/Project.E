#pragma once
#include "ComponentBase.h"


//*************************************
// �v���C���[�����C��
//*************************************
#define CAMERA_FIX              5.0f // �J�����C���l
#define PLAYER_ROT_UP          90.0f // �v���C���[�����C���@��
#define PLAYER_ROT_DOWN       270.0f // �v���C���[�����C���@��
#define PLAYER_ROT_LEFT         0.0f // �v���C���[�����C���@��
#define PLAYER_ROT_RIGHT      180.0f // �v���C���[�����C���@�E
#define PLAYER_ROT_UP_LEFT     45.0f // �v���C���[�����C���@����
#define PLAYER_ROT_UP_RIGHT   135.0f // �v���C���[�����C���@�E��
#define PLAYER_ROT_DOWN_LEFT  315.0f // �v���C���[�����C���@����
#define PLAYER_ROT_DOWN_RIGHT 225.0f // �v���C���[�����C���@�E��


//*************************************
// Player class
//*************************************
class Player : public ComponentBase
{
	//*******************************************
	// �ϐ�
	//*******************************************
protected:
	float mMaxSpeed; // �ő呬�x
	float mSpeed; // ���x
	float mRotSpeed; // ��]���x




	//*******************************************
	// �֐�
	//*******************************************
public:
	Player();
	~Player();

	void Init(GameObject*);
	void Update(float);
	void Draw();
	void Release();

	void SetMaxSpeed(float);
	void SetRotSpeed(float);

private:
	void KeyEvent(float);
	void Animation();
	void SpeedUp();
	void PositionUpdate();
	float ShortestAngle(float, float, float);
	float RotFix(float);

};


//*************************************
// �֐�
//*************************************

void PlayerInit();
void PlayerUpdate();
void PlayerRelease();