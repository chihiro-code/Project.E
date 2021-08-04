#include "Light.h"
#include "input.h"


//*************************************
// グローバル変数
//*************************************
Light* gpLight;


Light::Light()
{
	mViewMatrix = XMMatrixIdentity();
	mProjectionMatrix = XMMatrixIdentity();
	mPos   = { 0.0f, 0.0f, 0.0f };
	mFocus = { 0.0f, 0.0f, 0.0f };
	mColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	mProjection = { 0.0f, 0.0f, 0.0f, 0.0f };
	mMoveRot = 0;
}

Light::~Light()
{

}

XMMATRIX Light::GetViewMatrix()
{
	return mViewMatrix;
}

XMMATRIX Light::GetProjectionMatrix()
{
	return mProjectionMatrix;
}

XMFLOAT3 Light::GetPosition()
{
	return mPos;
}

XMFLOAT4 Light::GetColor()
{
	return mColor;
}

void Light::SetPosition(float x, float y, float z)
{
	mPos = { x, y, z };
}

void Light::SetFocus(float x, float y, float z)
{
	mFocus = { x, y, z };
}

void Light::SetColor(float r, float g, float b, float a)
{
	mColor = { r, g, b, a };
}

void Light::SetProjection(float fov, float aspect, float nerZ, float farZ)
{
	// プロジェクション変換行列
	mProjection.fov    = fov;
	mProjection.aspect = aspect;
	mProjection.nearZ  = nerZ;
	mProjection.farZ   = farZ;
	mProjectionMatrix = XMMatrixPerspectiveFovLH(mProjection.fov, mProjection.aspect, mProjection.nearZ, mProjection.farZ);
}

void Light::Init()
{
	// ビューセット
	ViewSet();
	
	// プロジェクションセット(初期値)
	mProjection.fov    = XMConvertToRadians(120.0f);
	mProjection.aspect = 1920.0f / 1080.0f;
	mProjection.nearZ  = 0.1f;
	mProjection.farZ   = 5000.0f;
	mProjectionMatrix  = XMMatrixPerspectiveFovLH(mProjection.fov, mProjection.aspect, mProjection.nearZ, mProjection.farZ);
}

void Light::Update()
{
	// 光源移動
	mPos.x = 0.0f;
	mPos.z = 0.0f;
	mPos.x -= XMScalarCos(XMConvertToRadians(mMoveRot)) * 100.0f; // X
	mPos.z -= XMScalarSin(XMConvertToRadians(mMoveRot)) * 100.0f; // Z
	mMoveRot += 0.1f;

	// 光源上下
	if (GetKeyboardPress(DIK_UP)) {
		mPos.y += 0.5f;
	}
	if (GetKeyboardPress(DIK_DOWN)) {
		mPos.y -= 0.5f;
	}

	ViewSet();
}

void Light::ViewSet()
{
	// ビュー更新
	FXMVECTOR pos      = { mPos.x, mPos.y, mPos.z };       // ライトがどこにあるか
	FXMVECTOR focus    = { mFocus.x, mFocus.y, mFocus.z }; // ライトがどこを見てるか
	FXMVECTOR upVector = { 0.0f, 1.0f, 0.0f };             // ライトの上下の向き
	mViewMatrix = XMMatrixLookAtLH(pos, focus, upVector);
}


//*************************************
// 関数
//*************************************

Light* GetLight()
{
	return gpLight;
}

void LightInit()
{
	gpLight = new Light;
	gpLight->SetPosition(0.0f, 50.0f, 0.0f);
	gpLight->SetFocus(0.0f, 5.0f, 0.0f);
	gpLight->SetColor(0.9f, 0.9f, 0.9f, 1.0f);
	gpLight->Init();

}

void LightUpdate()
{
	gpLight->Update();
}

void LightRelease()
{
	delete gpLight;
}
