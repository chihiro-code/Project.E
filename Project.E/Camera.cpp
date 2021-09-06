#include <windows.h>
#include "Camera.h"
#include "GameObject.h"
#include "input.h"


//*************************************
// グローバル変数
//*************************************
Camera* gpCamera;


//*************************************
// Camera関数
//*************************************

Camera::Camera()
{
	mCursorFlag = false;
	mRotX = 0.0f;
	mRotY = 0.0f;
}

Camera::~Camera()
{

}

void Camera::SetViewParam(FXMVECTOR pos, FXMVECTOR focus, FXMVECTOR up)
{
	// 数値保存
	mView.pos   = { pos.m128_f32[0], pos.m128_f32[1], pos.m128_f32[2] };
	mView.focus = { focus.m128_f32[0], focus.m128_f32[1], focus.m128_f32[2] };
	mView.up    = { up.m128_f32[0], up.m128_f32[1], up.m128_f32[2] };

	// ビューマトリックスへ適用
	mMatrixView = XMMatrixLookAtLH(pos, focus, up);
	mMatrixViewProjection = mMatrixView * mMatrixProjection;
}

void Camera::SetProjectionParam(const float fov, const float aspect, const float nearZ, const float farZ)
{
	// 数値保存
	mProjection.fov    = fov;
	mProjection.aspect = aspect;
	mProjection.nearZ  = nearZ;
	mProjection.farZ   = farZ;

	// プロジェクションマトリックスへ適用
	mMatrixProjection = XMMatrixPerspectiveFovLH(fov, aspect, nearZ, farZ);
	mMatrixViewProjection = mMatrixView * mMatrixProjection;
}

XMMATRIX Camera::GetProjectionMatrix(void)
{
	return mMatrixProjection;
}

XMMATRIX Camera::GetViewMatrix()
{
	return mMatrixView;
}

XMMATRIX Camera::GetViewProjectionMatrix()
{
	return mMatrixViewProjection;
}

XMFLOAT4 Camera::GetPosition(void)
{
	// カメラ位置を取得
	XMFLOAT4 viewPos = { mView.pos.x, mView.pos.y, mView.pos.z, 1.0f };

	return viewPos;
}

float Camera::GetRotY()
{
	return mRotY;
}

void Camera::MousUpdate(void)
{
	long mouseCurrStateX = GetMouseAxisX(); //マウスデータ取得(X軸移動)
	long mouseCurrStateY = GetMouseAxisY(); //マウスデータ取得(Y軸移動)

	static long mouseLastStateX = 0; //前回のマウスデータの格納位置(X軸移動)
	static long mouseLastStateY = 0; //前回のマウスデータの格納位置(Y軸移動)

	POINT CurrentPos; //現在のカーソル位置


	// マウスRotate Currnt 現在 Last 前
	if ((mouseCurrStateX != mouseLastStateX) || (mouseCurrStateY != mouseLastStateY)) {

		//X軸周りの回転
		mRotX += mouseCurrStateY * 0.1f;

		if (mRotX > 40.0f) {
			mRotX = 40.0f;
		}
		if (mRotX < -40.0f) {
			mRotX = -40.0f;
		}

		//Y軸周りの回転
		mRotY += mouseCurrStateX * 0.1f;

		mouseLastStateX = mouseCurrStateX;
		mouseLastStateY = mouseCurrStateY;
	}

	// マウスカーソル on of
	//CursorOnOf();

	// カーソル位置取得
	GetCursorPos(&CurrentPos); 

}

void Camera::CursorOnOf(void)
{
	if (GetKeyboardTrigger(DIK_LCONTROL)) {
		if (mCursorFlag == false) {
			mCursorFlag = true;
			ShowCursor(TRUE);
		}
		else if (mCursorFlag == true) {
			mCursorFlag = false;
			ShowCursor(FALSE);
		}
	}
}

void Camera::SetView(float posX, float posY, float posZ, float distance)
{
	mDistance = distance;

	FXMVECTOR posEye = {
			posX + XMScalarCos(XMConvertToRadians(180.0f - mRotY)) * mDistance,
			posY + XMScalarSin(XMConvertToRadians(mRotX))          * mDistance,
			posZ + XMScalarSin(XMConvertToRadians(180.0f - mRotY)) * mDistance
	}; // カメラがどこにあるか

	FXMVECTOR posFocus = { posX, posY, posZ }; // カメラがどこを見てるか
	FXMVECTOR upVector = { 0.0f, 1.0f, 0.0f }; // カメラの上下の向き

	// カメラ位置取得
	mPosEye = { posEye.m128_f32[0], posEye.m128_f32[1], posEye.m128_f32[2], 1.0f };

	// ビュー設定をビューマトリックスへ適用
	SetViewParam(posEye, posFocus, upVector);
}


//*************************************
// 関数
//*************************************

Camera* GetCamera()
{
	return gpCamera;
}

void CameraInit(void) {

	gpCamera = new Camera;

	// プロジェクション設定
	float fov    = XMConvertToRadians(70.0f);
	float aspect = 1920.0f / 1080.0f;
	float nearZ  = 0.1f;
	float farZ   = 5000.0f;
	gpCamera->SetProjectionParam(fov, aspect, nearZ, farZ);

	// ビュー設定
	FXMVECTOR pos   = { 0.0f, 0.0f, 0.0f }; // カメラがどこにあるか
	FXMVECTOR focus = { 0.0f, 1.0f, 0.0f }; // カメラがどこを見てるか
	FXMVECTOR up    = { 0.0f, 1.0f, 0.0f }; // カメラの上下の向き
	gpCamera->SetViewParam(pos, focus, up);

}

void CameraUpdate(void) {
	GameObject* GameObject = GetGameObject(PLAYER);

	// マウス更新
	gpCamera->MousUpdate();

	// カメラの位置、向き更新
	XMFLOAT3 pos = GameObject->PositionGet();
	gpCamera->SetView(pos.x, pos.y, pos.z, 30.0f);
	
}

void CameraRelease(void) {
	delete(gpCamera);
}