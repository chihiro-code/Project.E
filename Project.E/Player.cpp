#include "Player.h"
#include "input.h"
#include "Camera.h"
#include "GameObject.h"


//*************************************
// グローバル変数
//*************************************
Player* gpPlayer;


//*************************************
// Player関数
//*************************************
Player::Player()
{
	float mMaxSpeed = 1.0f; // 最大速度
	float mSpeed = 0.0f;    // 速度
	float mRotSpeed = 1.0f; // 回転速度
}

Player::~Player()
{

}

void Player::Init(GameObject* gameObject)
{
	// オブジェクトのアドレスを設定
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
	// 移動処理
	//-------------------------------------------
	static float rotY = 0.0f;
	XMFLOAT3 rot = mpOwner->RotationGet();
	// 角度修正
	cameraRotY = RotFix(cameraRotY);

	// 前左
	if (GetKeyboardPress(DIK_W) && GetKeyboardPress(DIK_A)) {
		SpeedUp();
		rotY = RotFix(PLAYER_ROT_UP_LEFT + cameraRotY);
	}
	// 前右
	else if (GetKeyboardPress(DIK_W) && GetKeyboardPress(DIK_D)) {
		SpeedUp();
		rotY = RotFix(PLAYER_ROT_UP_RIGHT + cameraRotY);
	}
	// 後ろ左
	else if (GetKeyboardPress(DIK_S) && GetKeyboardPress(DIK_A)) {
		SpeedUp();
		rotY = RotFix(PLAYER_ROT_DOWN_LEFT + cameraRotY);
	}
	// 後ろ右
	else if (GetKeyboardPress(DIK_S) && GetKeyboardPress(DIK_D)) {
		SpeedUp();
		rotY = RotFix(PLAYER_ROT_DOWN_RIGHT + cameraRotY);
	}
	// 前
	else if (GetKeyboardPress(DIK_W)) {
		SpeedUp();
		rotY = RotFix(PLAYER_ROT_UP + cameraRotY);
	}
	// 後ろ
	else if (GetKeyboardPress(DIK_S)) {
		SpeedUp();
		rotY = RotFix(PLAYER_ROT_DOWN + cameraRotY);
	}
	// 左
	else if (GetKeyboardPress(DIK_A)) {
		SpeedUp();
		rotY = RotFix(PLAYER_ROT_LEFT + cameraRotY);		
	}
	// 右
	else if (GetKeyboardPress(DIK_D)) {
		SpeedUp();
		rotY = RotFix(PLAYER_ROT_RIGHT + cameraRotY);		
	}

	// 指定角度に最短で向く方向を判定し、回転スピード分回る
	rot.y = ShortestAngle(rotY, rot.y, mRotSpeed);
	// 指定した角度に近づいたら指定角度へ（挙動修正）
	if (rot.y > (rotY - CAMERA_FIX) && rot.y < (rotY + CAMERA_FIX)) {
		rot.y = rotY;
	}
	// 向き変更
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
	// playerの向き取得
	XMFLOAT3 rot =  mpOwner->RotationGet();
	XMMATRIX matrixRotY = XMMatrixRotationY(XMConvertToRadians(rot.y));
	// 向いている方向と移動速度で現在位置を更新 
	// ベクトルの回転
	XMVECTOR speedVector = { 0.0f, 0.0f, mSpeed };              // スピードをXMVECTOR型に直す
	speedVector = XMVector3Transform(speedVector, matrixRotY);  // スピードベクトルとＹ軸回転行列の掛け算＝結果は回転されたスピード
	XMFLOAT3 pos = mpOwner->PositionGet();
	pos.x += speedVector.m128_f32[0]; // m128_f32[0]はfloat型のＸの値
	pos.z += speedVector.m128_f32[2]; // m128_f32[2]はfloat型のＺの値
	mpOwner->PositionSet(pos.x, pos.y, pos.z);

}

float Player::ShortestAngle(float targetRot, float currentRot, float valueRot)
{
	if (currentRot > 360.0f) { currentRot = currentRot - 360.0f/*(currentRot / 360.0f * 360.0f)*/; }
	else if (currentRot < 0.0f) { currentRot += 360.0f; }

	if (fabsf(targetRot - currentRot) <= 180.0f) { // １８０度以内なら
		if ((targetRot - currentRot) > 0) { // +なら
			currentRot += valueRot;
		}
		else if ((targetRot - currentRot) < 0) { // -なら
			currentRot -= valueRot;
		}
	}
	if (fabsf(targetRot - currentRot) >= 180.0f) { // １８０度以上なら
		if ((targetRot - currentRot) > 0) { // +なら
			currentRot -= valueRot;
		}
		else if ((targetRot - currentRot) < 0) { // -なら
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
// 関数
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
