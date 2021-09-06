#include <windows.h>
#include "Camera.h"
#include "GameObject.h"
#include "input.h"


//*************************************
// �O���[�o���ϐ�
//*************************************
Camera* gpCamera;


//*************************************
// Camera�֐�
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
	// ���l�ۑ�
	mView.pos   = { pos.m128_f32[0], pos.m128_f32[1], pos.m128_f32[2] };
	mView.focus = { focus.m128_f32[0], focus.m128_f32[1], focus.m128_f32[2] };
	mView.up    = { up.m128_f32[0], up.m128_f32[1], up.m128_f32[2] };

	// �r���[�}�g���b�N�X�֓K�p
	mMatrixView = XMMatrixLookAtLH(pos, focus, up);
	mMatrixViewProjection = mMatrixView * mMatrixProjection;
}

void Camera::SetProjectionParam(const float fov, const float aspect, const float nearZ, const float farZ)
{
	// ���l�ۑ�
	mProjection.fov    = fov;
	mProjection.aspect = aspect;
	mProjection.nearZ  = nearZ;
	mProjection.farZ   = farZ;

	// �v���W�F�N�V�����}�g���b�N�X�֓K�p
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
	// �J�����ʒu���擾
	XMFLOAT4 viewPos = { mView.pos.x, mView.pos.y, mView.pos.z, 1.0f };

	return viewPos;
}

float Camera::GetRotY()
{
	return mRotY;
}

void Camera::MousUpdate(void)
{
	long mouseCurrStateX = GetMouseAxisX(); //�}�E�X�f�[�^�擾(X���ړ�)
	long mouseCurrStateY = GetMouseAxisY(); //�}�E�X�f�[�^�擾(Y���ړ�)

	static long mouseLastStateX = 0; //�O��̃}�E�X�f�[�^�̊i�[�ʒu(X���ړ�)
	static long mouseLastStateY = 0; //�O��̃}�E�X�f�[�^�̊i�[�ʒu(Y���ړ�)

	POINT CurrentPos; //���݂̃J�[�\���ʒu


	// �}�E�XRotate Currnt ���� Last �O
	if ((mouseCurrStateX != mouseLastStateX) || (mouseCurrStateY != mouseLastStateY)) {

		//X������̉�]
		mRotX += mouseCurrStateY * 0.1f;

		if (mRotX > 40.0f) {
			mRotX = 40.0f;
		}
		if (mRotX < -40.0f) {
			mRotX = -40.0f;
		}

		//Y������̉�]
		mRotY += mouseCurrStateX * 0.1f;

		mouseLastStateX = mouseCurrStateX;
		mouseLastStateY = mouseCurrStateY;
	}

	// �}�E�X�J�[�\�� on of
	//CursorOnOf();

	// �J�[�\���ʒu�擾
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
	}; // �J�������ǂ��ɂ��邩

	FXMVECTOR posFocus = { posX, posY, posZ }; // �J�������ǂ������Ă邩
	FXMVECTOR upVector = { 0.0f, 1.0f, 0.0f }; // �J�����̏㉺�̌���

	// �J�����ʒu�擾
	mPosEye = { posEye.m128_f32[0], posEye.m128_f32[1], posEye.m128_f32[2], 1.0f };

	// �r���[�ݒ���r���[�}�g���b�N�X�֓K�p
	SetViewParam(posEye, posFocus, upVector);
}


//*************************************
// �֐�
//*************************************

Camera* GetCamera()
{
	return gpCamera;
}

void CameraInit(void) {

	gpCamera = new Camera;

	// �v���W�F�N�V�����ݒ�
	float fov    = XMConvertToRadians(70.0f);
	float aspect = 1920.0f / 1080.0f;
	float nearZ  = 0.1f;
	float farZ   = 5000.0f;
	gpCamera->SetProjectionParam(fov, aspect, nearZ, farZ);

	// �r���[�ݒ�
	FXMVECTOR pos   = { 0.0f, 0.0f, 0.0f }; // �J�������ǂ��ɂ��邩
	FXMVECTOR focus = { 0.0f, 1.0f, 0.0f }; // �J�������ǂ������Ă邩
	FXMVECTOR up    = { 0.0f, 1.0f, 0.0f }; // �J�����̏㉺�̌���
	gpCamera->SetViewParam(pos, focus, up);

}

void CameraUpdate(void) {
	GameObject* GameObject = GetGameObject(PLAYER);

	// �}�E�X�X�V
	gpCamera->MousUpdate();

	// �J�����̈ʒu�A�����X�V
	XMFLOAT3 pos = GameObject->PositionGet();
	gpCamera->SetView(pos.x, pos.y, pos.z, 30.0f);
	
}

void CameraRelease(void) {
	delete(gpCamera);
}