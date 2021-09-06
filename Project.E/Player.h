#pragma once
#include "ComponentBase.h"


//*************************************
// プレイヤー向き修正
//*************************************
#define CAMERA_FIX              5.0f // カメラ修正値
#define PLAYER_ROT_UP          90.0f // プレイヤー向き修正　上
#define PLAYER_ROT_DOWN       270.0f // プレイヤー向き修正　下
#define PLAYER_ROT_LEFT         0.0f // プレイヤー向き修正　左
#define PLAYER_ROT_RIGHT      180.0f // プレイヤー向き修正　右
#define PLAYER_ROT_UP_LEFT     45.0f // プレイヤー向き修正　左上
#define PLAYER_ROT_UP_RIGHT   135.0f // プレイヤー向き修正　右上
#define PLAYER_ROT_DOWN_LEFT  315.0f // プレイヤー向き修正　左下
#define PLAYER_ROT_DOWN_RIGHT 225.0f // プレイヤー向き修正　右下


//*************************************
// Player class
//*************************************
class Player : public ComponentBase
{
	//*******************************************
	// 変数
	//*******************************************
protected:
	float mMaxSpeed; // 最大速度
	float mSpeed; // 速度
	float mRotSpeed; // 回転速度




	//*******************************************
	// 関数
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
// 関数
//*************************************

void PlayerInit();
void PlayerUpdate();
void PlayerRelease();