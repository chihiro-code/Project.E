#pragma once
#include <cstdio>
#include "DirectXMath.h"


using namespace DirectX;


//*************************************
// Light class
//*************************************
class Light 
{
	//*******************************************
	// 変数
	//*******************************************
private:

	// プロジェクション
	struct Projection {
		float fov;
		float aspect;
		float nearZ;
		float farZ;
	};

	XMMATRIX mViewMatrix;
	XMMATRIX mProjectionMatrix;
	XMFLOAT3 mPos;
	XMFLOAT3 mFocus;
	XMFLOAT4 mColor;
	Projection mProjection;
	float mMoveRot;


	//*******************************************
	// 関数
	//*******************************************
public:

	Light();
	~Light();

	XMMATRIX GetViewMatrix();
	XMMATRIX GetProjectionMatrix();
	XMFLOAT3 GetPosition();
	XMFLOAT4 GetColor();
	void SetPosition(float, float, float);
	void SetFocus(float, float, float);
	void SetColor(float, float, float, float);
	void SetProjection(float, float, float, float);
	void Init();
	void Update();
	
private:

	void ViewSet();

};


//*************************************
// 関数
//*************************************

// ゲット関数
Light* GetLight();

// ライト初期化
void LightInit();

// ライト更新
void LightUpdate();

// ライト解放
void LightRelease();

