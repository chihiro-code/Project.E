#pragma once
#include <DirectXMath.h>


using namespace DirectX;


//*************************************
// Camera class
//*************************************
class Camera
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
	// ビュー
	struct View {
		XMFLOAT3 pos;
		XMFLOAT3 focus;
		XMFLOAT3 up;
	};

	XMMATRIX mMatrixProjection;
	XMMATRIX mMatrixView;
	XMMATRIX mMatrixViewProjection;
	XMFLOAT4 mPosEye;

	Projection mProjection;
	View mView;

	float mRotX;
	float mRotY;
	float mDistance;
	bool mCursorFlag;



	//*******************************************
	// 関数
	//*******************************************
public:

	Camera();
	~Camera();

	// セット関数
	// ビュー設定をビューマトリックスへ適用
	void SetViewParam(FXMVECTOR posEye, FXMVECTOR posFocus, FXMVECTOR upVector);
	// プロジェクション設定をプロジェクションマトリックスへ適用
	void SetProjectionParam(const float fov, const float aspect, const float nearZ, const float farZ);
	
	// ゲット関数
	XMMATRIX GetProjectionMatrix();
	XMMATRIX GetViewMatrix();
	XMMATRIX GetViewProjectionMatrix();
	XMFLOAT4 GetPosition();

	// マウス更新
	void MousUpdate(void);
	// カーソル表示非表示
	void CursorOnOf(void);
	// カメラの位置、向き設定
	void SetView(float posX, float posY, float posZ, float distance);

};


//*************************************
// 関数
//*************************************

// ゲット関数
Camera* GetCamera();

// カメラ初期化
void CameraInit();
// カメラ更新
void CameraUpdate();
// カメラ解放
void CameraRelease();