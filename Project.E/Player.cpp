#include "Player.h"
#include "input.h"
#include "Camera.h"
#include "GameObject.h"


//*************************************
// �O���[�o���ϐ�
//*************************************
Player* gpPlayer;


//*************************************
// Player�֐�
//*************************************
Player::Player()
{
	float mMaxSpeed = 1.0f; // �ő呬�x
	float mSpeed = 0.0f;    // ���x
	float mRotSpeed = 1.0f; // ��]���x
}

Player::~Player()
{

}

void Player::Init(GameObject* gameObject)
{
	// �I�u�W�F�N�g�̃A�h���X��ݒ�
	SetOwner(gameObject);

}

void Player::Update(float cameraRotY)
{
	KeyEvent(cameraRotY);
	PositionUpdate();
	Animation();

	mSpeed -= 0.01f;
	if (mSpeed < 0) {
		mSpeed = 0;
	}
}

void Player::Draw()
{

}

void Player::Release()
{

}

void Player::SetMaxSpeed(float maxSpeed)
{
	mMaxSpeed = maxSpeed;
}

void Player::SetRotSpeed(float rotSpeed)
{
	mRotSpeed = rotSpeed;
}

void Player::KeyEvent(float cameraRotY)
{
	//-------------------------------------------
	// �ړ�����
	//-------------------------------------------
	static float rotY = 0.0f;
	XMFLOAT3 rot = mpOwner->RotationGet();
	// �p�x�C��
	cameraRotY = RotFix(cameraRotY);

	// �O��
	if (GetKeyboardPress(DIK_W) && GetKeyboardPress(DIK_A)) {
		SpeedUp();
		rotY = RotFix(PLAYER_ROT_UP_LEFT + cameraRotY);
	}
	// �O�E
	else if (GetKeyboardPress(DIK_W) && GetKeyboardPress(DIK_D)) {
		SpeedUp();
		rotY = RotFix(PLAYER_ROT_UP_RIGHT + cameraRotY);
	}
	// ��덶
	else if (GetKeyboardPress(DIK_S) && GetKeyboardPress(DIK_A)) {
		SpeedUp();
		rotY = RotFix(PLAYER_ROT_DOWN_LEFT + cameraRotY);
	}
	// ���E
	else if (GetKeyboardPress(DIK_S) && GetKeyboardPress(DIK_D)) {
		SpeedUp();
		rotY = RotFix(PLAYER_ROT_DOWN_RIGHT + cameraRotY);
	}
	// �O
	else if (GetKeyboardPress(DIK_W)) {
		SpeedUp();
		rotY = RotFix(PLAYER_ROT_UP + cameraRotY);
	}
	// ���
	else if (GetKeyboardPress(DIK_S)) {
		SpeedUp();
		rotY = RotFix(PLAYER_ROT_DOWN + cameraRotY);
	}
	// ��
	else if (GetKeyboardPress(DIK_A)) {
		SpeedUp();
		rotY = RotFix(PLAYER_ROT_LEFT + cameraRotY);		
	}
	// �E
	else if (GetKeyboardPress(DIK_D)) {
		SpeedUp();
		rotY = RotFix(PLAYER_ROT_RIGHT + cameraRotY);		
	}

	// �w��p�x�ɍŒZ�Ō��������𔻒肵�A��]�X�s�[�h�����
	rot.y = ShortestAngle(rotY, rot.y, mRotSpeed);
	// �w�肵���p�x�ɋ߂Â�����w��p�x�ցi�����C���j
	if (rot.y > (rotY - CAMERA_FIX) && rot.y < (rotY + CAMERA_FIX)) {
		rot.y = rotY;
	}
	// �����ύX
	mpOwner->RotationSet(rot.x, rot.y, rot.z);

}

void Player::Animation()
{
	if (0 < mSpeed) {
		mpOwner->AnimationUpdate(0, 30, 10.0f * mSpeed);
	}
}

void Player::SpeedUp()
{
	mSpeed += 0.1f;

	if (mSpeed > mMaxSpeed) {
		mSpeed = mMaxSpeed;
	}
	if (mSpeed < 0) {
		mSpeed += 0.01f;
	}
}

void Player::PositionUpdate()
{
	// player�̌����擾
	XMFLOAT3 rot =  mpOwner->RotationGet();
	XMMATRIX matrixRotY = XMMatrixRotationY(XMConvertToRadians(rot.y));
	// �����Ă�������ƈړ����x�Ō��݈ʒu���X�V 
	// �x�N�g���̉�]
	XMVECTOR speedVector = { 0.0f, 0.0f, mSpeed };              // �X�s�[�h��XMVECTOR�^�ɒ���
	speedVector = XMVector3Transform(speedVector, matrixRotY);  // �X�s�[�h�x�N�g���Ƃx����]�s��̊|���Z�����ʂ͉�]���ꂽ�X�s�[�h
	XMFLOAT3 pos = mpOwner->PositionGet();
	pos.x += speedVector.m128_f32[0]; // m128_f32[0]��float�^�̂w�̒l
	pos.z += speedVector.m128_f32[2]; // m128_f32[2]��float�^�̂y�̒l
	mpOwner->PositionSet(pos.x, pos.y, pos.z);

}

float Player::ShortestAngle(float targetRot, float currentRot, float valueRot)
{
	if (currentRot > 360.0f) { currentRot = currentRot - 360.0f/*(currentRot / 360.0f * 360.0f)*/; }
	else if (currentRot < 0.0f) { currentRot += 360.0f; }

	if (fabsf(targetRot - currentRot) <= 180.0f) { // �P�W�O�x�ȓ��Ȃ�
		if ((targetRot - currentRot) > 0) { // +�Ȃ�
			currentRot += valueRot;
		}
		else if ((targetRot - currentRot) < 0) { // -�Ȃ�
			currentRot -= valueRot;
		}
	}
	if (fabsf(targetRot - currentRot) >= 180.0f) { // �P�W�O�x�ȏ�Ȃ�
		if ((targetRot - currentRot) > 0) { // +�Ȃ�
			currentRot -= valueRot;
		}
		else if ((targetRot - currentRot) < 0) { // -�Ȃ�
			currentRot += valueRot;
		}
	}

	return currentRot;
}

float Player::RotFix(float rot)
{
	if (rot > 360.0f) {
		while (rot > 360.0f) {
			rot -= 360.0f;
		}
	}
	else if (rot < 0.0f) {
		while (rot < 0.0f) {
			rot += 360.0f;
		}
	}

	return rot;
}


//*************************************
// �֐�
//*************************************

void PlayerInit()
{
	GameObject* gameObject = GetGameObject(PLAYER);

	gpPlayer = new Player;
	gpPlayer->Init(gameObject);
	gpPlayer->SetMaxSpeed(1.0f);
	gpPlayer->SetRotSpeed(2.0f);

}

void PlayerUpdate()
{
	Camera* camera = GetCamera();

	gpPlayer->Update(camera->GetRotY());

}

void PlayerRelease()
{
	delete gpPlayer;
}
