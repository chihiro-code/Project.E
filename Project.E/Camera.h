#pragma once
#include <DirectXMath.h>


using namespace DirectX;


//*************************************
// Camera class
//*************************************
class Camera
{
	//*******************************************
	// �ϐ�
	//*******************************************
private:

	// �v���W�F�N�V����
	struct Projection {
		float fov;
		float aspect;
		float nearZ;
		float farZ;
	};
	// �r���[
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
	// �֐�
	//*******************************************
public:

	Camera();
	~Camera();

	// �Z�b�g�֐�
	// �r���[�ݒ���r���[�}�g���b�N�X�֓K�p
	void SetViewParam(FXMVECTOR posEye, FXMVECTOR posFocus, FXMVECTOR upVector);
	// �v���W�F�N�V�����ݒ���v���W�F�N�V�����}�g���b�N�X�֓K�p
	void SetProjectionParam(const float fov, const float aspect, const float nearZ, const float farZ);
	
	// �Q�b�g�֐�
	XMMATRIX GetProjectionMatrix();
	XMMATRIX GetViewMatrix();
	XMMATRIX GetViewProjectionMatrix();
	XMFLOAT4 GetPosition();

	// �}�E�X�X�V
	void MousUpdate(void);
	// �J�[�\���\����\��
	void CursorOnOf(void);
	// �J�����̈ʒu�A�����ݒ�
	void SetView(float posX, float posY, float posZ, float distance);

};


//*************************************
// �֐�
//*************************************

// �Q�b�g�֐�
Camera* GetCamera();

// �J����������
void CameraInit();
// �J�����X�V
void CameraUpdate();
// �J�������
void CameraRelease();